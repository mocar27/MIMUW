#include <sys/wait.h>
#include <poll.h>

#include "radio_list.h"
#include "common.h"
#include "err.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t play = PTHREAD_COND_INITIALIZER;
atomic_bool is_playing = false;

pthread_cond_t was_empty = PTHREAD_COND_INITIALIZER;
atomic_ullong buffer_fulfill = 0;
atomic_ullong read_index = 0;

char* discover_address = DEFAULT_DISCOVER_ADDR;
radio_station* radio_list = NULL;
radio_station* current_station = NULL;
atomic_int socket_fd = -1;
pthread_mutex_t radio_list_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t no_socket = PTHREAD_COND_INITIALIZER;

uint16_t control_port = DEFAULT_CONTROL_PORT;
uint16_t ui_port = DEFAULT_UI_PORT;
char* favourite_station_name = NULL;
atomic_bool show_ui = false;

uint8_t* main_buffer = NULL;
uint64_t buffer_size = DEFAULT_BSIZE;
atomic_ullong packet_size = 0;
uint64_t r_time = DEFAULT_RTIME;

uint64_t last_session_id = 0;
uint64_t byte_0 = 0;

void* ui_thread(void* data) {

    // Dynamic defined array of file descriptors to exponentially accept connections for poll.
    int max_connections = 2;
    struct pollfd* fds = calloc(max_connections, sizeof(struct pollfd));
    ENSURE(fds != NULL);

    for (int i = 0; i < max_connections; i++) {
        fds[i].fd = -1;
        fds[i].events = POLLIN;
        fds[i].revents = 0;
    }

    // Setting socket for handling TCP UI connections.
    fds[0].fd = open_socket_tcp();
    bind_socket(fds[0].fd, ui_port);
    start_listening(fds[0].fd, QUEUE_LENGTH);

    for (;;) {
        for (int i = 0; i < max_connections; i++) {
            fds[i].revents = 0;
        }

        int poll_res = poll(fds, max_connections, 0);
        if (poll_res < 0) {
            PRINT_ERRNO();
        }
        else if (poll_res > 0) {
            // New TCP client (for example telnet) connection.
            if (fds[0].revents & POLLIN) {
                int client_fd = accept_connection(fds[0].fd, NULL);
                bool accepted = false;

                for (int i = 1; i < max_connections; ++i) {
                    if (fds[i].fd == -1) {
                        fds[i].fd = client_fd;
                        fds[i].events = POLLIN;
                        accepted = true;
                        break;
                    }
                }
                // If current clients limit is reached, resize clients' array and accept connection.
                if (!accepted) {
                    fds = realloc(fds, 2 * max_connections * sizeof(struct pollfd));
                    for (int i = max_connections; i < 2 * max_connections; i++) {
                        fds[i].fd = -1;
                        fds[i].events = POLLIN;
                        fds[i].revents = 0;
                    }
                    fds[max_connections].fd = client_fd;
                    fds[max_connections].events = POLLIN;
                    max_connections *= 2;
                    ENSURE(fds != NULL);
                }

                // Show UI for newly connected client.
                CHECK_ERRNO(pthread_mutex_lock(&radio_list_mutex));

                radio_station* temp = radio_list;
                char* buf = construct_user_interface(temp, current_station);

                CHECK_ERRNO(pthread_mutex_unlock(&radio_list_mutex));
                ssize_t bytes_written = writen(client_fd, buf, strlen(buf));
                ENSURE(bytes_written >= 0);
                free(buf);
            }

            bool show_ui_switch;
            char* user_interface;

            // Retrieving getting (messages) input in our connections.
            for (int i = 1; i < max_connections; i++) {
                show_ui_switch = false;
                user_interface = NULL;
                if (fds[i].fd != -1 && (fds[i].revents & (POLLIN | POLLERR))) {
                    char buffer[MAX_PSIZE];
                    ssize_t read_length = read(fds[i].fd, buffer, MAX_PSIZE);

                    if (read_length <= 0) {
                        CHECK_ERRNO(close(fds[i].fd));
                        fds[i].fd = -1;
                    }
                    // If the message was sent on the connection, valide if it's UP or DOWN arrow key.
                    else if (read_length > 0) {
                        for (int i = 0; i < read_length; i++) {
                            // If UP or DOWN arrow key was pressed change radio station.
                            if (buffer[i] == 27 && buffer[i + 1] == 91) {
                                CHECK_ERRNO(pthread_mutex_lock(&radio_list_mutex));
                                if (buffer[i + 2] == UP_ARROW) {
                                    if (current_station != NULL) {
                                        radio_station* temp = get_previous_radio_station(&radio_list, current_station);
                                        if (temp != NULL && temp != current_station) {
                                            current_station = temp;
                                            socket_fd = current_station->station_socket;
                                            memset(main_buffer, 0, buffer_size);
                                            is_playing = false;
                                            read_index = 0;
                                            buffer_fulfill = 0;
                                            last_session_id = 0;
                                            show_ui_switch = true;
                                            user_interface = construct_user_interface(radio_list, current_station);
                                        }
                                    }
                                }
                                else if (buffer[i + 2] == DOWN_ARROW) {
                                    if (current_station == NULL) {
                                        if (radio_list != NULL) {
                                            current_station = radio_list;
                                            socket_fd = current_station->station_socket;
                                            memset(main_buffer, 0, buffer_size);
                                            is_playing = false;
                                            read_index = 0;
                                            buffer_fulfill = 0;
                                            last_session_id = 0;
                                            show_ui_switch = true;
                                            user_interface = construct_user_interface(radio_list, current_station);
                                            CHECK_ERRNO(pthread_cond_signal(&no_socket));
                                        }
                                    }
                                    else if (current_station->next_station != NULL) {
                                        current_station = current_station->next_station;
                                        socket_fd = current_station->station_socket;
                                        memset(main_buffer, 0, buffer_size);
                                        is_playing = false;
                                        read_index = 0;
                                        buffer_fulfill = 0;
                                        last_session_id = 0;
                                        show_ui_switch = true;
                                        user_interface = construct_user_interface(radio_list, current_station);
                                    }
                                }
                            }
                            CHECK_ERRNO(pthread_mutex_unlock(&radio_list_mutex));
                        }
                    }
                    // If radio station was switched show UI for all clients once again.
                    if (show_ui_switch) {
                        for (int i = 1; i < max_connections; i++) {
                            if (fds[i].fd != -1) {
                                ssize_t bytes_written = writen(fds[i].fd, user_interface, strlen(user_interface));
                                ENSURE(bytes_written >= 0);
                            }
                        }
                        free(user_interface);
                    }
                }
            }
        }
        // If there was a change in radio list display new UI for all clients.
        if (show_ui) {
            show_ui = false;
            CHECK_ERRNO(pthread_mutex_lock(&radio_list_mutex));
            char* user_interface = construct_user_interface(radio_list, current_station);
            CHECK_ERRNO(pthread_mutex_unlock(&radio_list_mutex));
            for (int i = 1; i < max_connections; i++) {
                if (fds[i].fd != -1) {
                    ssize_t bytes_written = writen(fds[i].fd, user_interface, strlen(user_interface));
                    ENSURE(bytes_written >= 0);
                }
            }
            free(user_interface);
        }
    }

    return 0;
}

void* writing_thread(void* data) {

    for (;;) {

        CHECK_ERRNO(pthread_mutex_lock(&mutex));
        while (!is_playing) {
            CHECK_ERRNO(pthread_cond_wait(&play, &mutex));
        }
        while (buffer_fulfill == 0) {
            CHECK_ERRNO(pthread_cond_wait(&was_empty, &mutex));
        }

        ssize_t bytes_written = writen(STDOUT_FILENO, main_buffer + read_index, packet_size);
        ENSURE(bytes_written >= 0);

        read_index += packet_size;
        buffer_fulfill -= packet_size;
        if (read_index + packet_size > buffer_size) {
            read_index = 0;
        }

        CHECK_ERRNO(pthread_mutex_unlock(&mutex));
    }

    return 0;
}

void* control_thread_receiving(void* data) {

    int* control_socket = data;

    // Thread receiving REPLAY messages.
    for (;;) {
        char received_message[MAX_PSIZE];
        memset(received_message, 0, MAX_PSIZE);
        bool processing = true;

        // Receive control message and validate it's characters.
        ssize_t received_length = receive_message(*control_socket, received_message, MAX_PSIZE, NO_FLAGS);
        ENSURE(received_length >= 0);

        for (int i = 0; i < received_length; i++) {
            if (received_message[i] < MIN_ASCII || received_message[i] > MAX_ASCII) {
                if (received_message[i] != '\n') {
                    processing = false;
                    break;
                }
            }
        }

        if (processing) {
            char* message_type = calloc(REPLAY_MESSAGE_SIZE, sizeof(char));
            ENSURE(message_type != NULL);
            CHECK_ERRNO(memcpy(message_type, received_message, REPLAY_MESSAGE_SIZE - 1));

            // Validate if received REPLAY message is correct.
            if (strcmp(message_type, REPLAY_MESSAGE) == 0) {
                char* host = calloc(INET_ADDRSTRLEN, sizeof(char));
                ENSURE(host != NULL);
                char* data_port_str = calloc(MAX_PORT_LEN + 1, sizeof(char));
                ENSURE(data_port_str != NULL);
                char* station_name = calloc(MAX_NAME_LENGTH + 1, sizeof(char));
                ENSURE(station_name != NULL);

                processing = parse_message_arguments(received_message, &host, &data_port_str, &station_name);

                // If all arguments are correct, update radio list.
                if (processing) {
                    uint16_t receive_data_port = read_port(data_port_str);

                    CHECK_ERRNO(pthread_mutex_lock(&radio_list_mutex));

                    radio_station* check_client = update_lookup_time(&radio_list, host, receive_data_port, station_name, &show_ui);

                    if (socket_fd == -1 && check_client != NULL) {
                        if (favourite_station_name != NULL && strcmp(favourite_station_name, station_name) == 0) {
                            current_station = check_client;
                            socket_fd = current_station->station_socket;
                            memset(main_buffer, 0, buffer_size);
                            is_playing = false;
                            read_index = 0;
                            buffer_fulfill = 0;
                            last_session_id = 0;
                            CHECK_ERRNO(pthread_cond_signal(&no_socket));
                        }
                        else if (favourite_station_name == NULL) {
                            current_station = check_client;
                            socket_fd = current_station->station_socket;
                            memset(main_buffer, 0, buffer_size);
                            is_playing = false;
                            read_index = 0;
                            buffer_fulfill = 0;
                            last_session_id = 0;
                            CHECK_ERRNO(pthread_cond_signal(&no_socket));
                        }
                    }
                    CHECK_ERRNO(pthread_mutex_unlock(&radio_list_mutex));
                    free(host);
                    free(data_port_str);
                    free(station_name);
                }
            }
        }
    }

    return 0;
}

void* control_thread_sending(void* data) {

    int control_socket = open_socket_udp();

    // Setting socket for sending and receiving control messages.
    // Additionally creating thread for receiving control messages and passing pointer to created socket.
    int optval = 1;
    CHECK_ERRNO(setsockopt(control_socket, SOL_SOCKET, SO_BROADCAST, (void*)&optval, sizeof optval));
    optval = TTL_VALUE;
    CHECK_ERRNO(setsockopt(control_socket, IPPROTO_IP, IP_MULTICAST_TTL, (void*)&optval, sizeof optval));

    struct sockaddr_in remote_address;
    remote_address.sin_family = AF_INET;
    remote_address.sin_port = htons(control_port);
    CHECK_ERRNO(inet_pton(AF_INET, discover_address, &remote_address.sin_addr));

    pthread_t control_thread_receiving_id;
    CHECK_ERRNO(pthread_create(&control_thread_receiving_id, NULL, control_thread_receiving, &control_socket));

    // Thread sending LOOKUP requests.
    for (;;) {
        sleep(LOOKUP_SLEEP_TIME);
        CHECK_ERRNO(pthread_mutex_lock(&radio_list_mutex));

        if (radio_list != NULL) {
            bool check_station = remove_inactive_stations(&radio_list, current_station, &radio_list_mutex, &show_ui);
            // If current station is inactive.
            if (!check_station) {
                // If favourite station is defined.
                if (favourite_station_name != NULL) {
                    radio_station* check_client = find_radio_station_by_name(&radio_list, favourite_station_name);

                    // If favourite station is on the list.
                    if (check_client != NULL && check_client != current_station) {
                        socket_fd = check_client->station_socket;
                        remove_radio_station(&radio_list, current_station);
                        current_station = check_client;
                    }
                    // If favourite station is not on the list, listen to nobody and wait for favourite.
                    else {
                        socket_fd = -1;
                        remove_radio_station(&radio_list, current_station);
                        current_station = NULL;
                    }
                }
                // If favourite station in not defiened.
                else {
                    // If current station is not the first on the list, listen to the first.
                    if (radio_list != current_station) {
                        socket_fd = radio_list->station_socket;
                        CHECK_ERRNO(remove_radio_station(&radio_list, current_station));
                        current_station = radio_list;
                    }
                    else {
                        // If current station is the first on the list and there is another station, listen to the next.
                        if (current_station->next_station != NULL) {
                            socket_fd = current_station->next_station->station_socket;
                            CHECK_ERRNO(remove_radio_station(&radio_list, current_station));
                            current_station = radio_list;
                        }
                        // If current station is the first on the list and there is no another station, listen to nobody.
                        else {
                            socket_fd = -1;
                            CHECK_ERRNO(remove_radio_station(&radio_list, current_station));
                            current_station = NULL;
                        }
                    }
                }
                show_ui = true;
                memset(main_buffer, 0, buffer_size);
                is_playing = false;
                read_index = 0;
                buffer_fulfill = 0;
                last_session_id = 0;
            }
        }
        CHECK_ERRNO(pthread_mutex_unlock(&radio_list_mutex));

        // Sending LOOKUP message asking senders to indentify themselves.
        char* message = LOOKUP_MESSAGE;
        ssize_t bytes_sent = sendto(control_socket, message, LOOKUP_MESSAGE_SIZE, NO_FLAGS,
            (struct sockaddr*)&remote_address, sizeof(remote_address));
        ENSURE(bytes_sent >= 0);
    }

    CHECK_ERRNO(pthread_join(control_thread_receiving_id, NULL));
    CHECK_ERRNO(close(control_socket));
    return 0;
}

int main(int argc, char** argv) {

    int opt;
    opterr = 0;
    while ((opt = getopt(argc, argv, "d:C:U:b:R:n:")) != -1) {
        switch (opt) {
        case 'd':
            discover_address = optarg;
            break;
        case 'C':
            control_port = read_port(optarg);
            break;
        case 'U':
            ui_port = read_port(optarg);
            break;
        case 'b':
            buffer_size = (uint64_t)strtoull(optarg, NULL, 10);
            break;
        case 'R':
            r_time = (uint64_t)strtoull(optarg, NULL, 10);
            break;
        case 'n':
            favourite_station_name = optarg;
            break;
        case '?':
            if (optopt == 'd' || optopt == 'C' || optopt == 'U' ||
                optopt == 'b' || optopt == 'R' || optopt == 'n') {
                fatal("Option %s requires an argument!", argv[optind - 1]);
            }
        default:
            fatal("Invalid option or argument: %s", argv[optind - 1]);
        }
    }

    validate_arguments(discover_address, favourite_station_name, r_time);
    if ((int)buffer_size < 1 || buffer_size > MAX_BSIZE) {
        fatal("Buffer size must be between 1 and UINT64_MAX!");
    }

    pthread_mutex_t local_mutex = PTHREAD_MUTEX_INITIALIZER;

    // Main buffer for both threads, music data is here.
    main_buffer = calloc(buffer_size, sizeof(uint8_t));
    ENSURE(main_buffer != NULL);
    uint64_t initial_buffer_size = buffer_size;

    // Values session_id and first_byte_num received in one package.
    uint64_t temp_session_id = 0;
    uint64_t received_byte_num = 0;

    pthread_t writing_thread_id;
    CHECK_ERRNO(pthread_create(&writing_thread_id, NULL, writing_thread, NULL));
    pthread_t control_thread_sending_id;
    CHECK_ERRNO(pthread_create(&control_thread_sending_id, NULL, control_thread_sending, NULL));
    pthread_t ui_thread_id;
    CHECK_ERRNO(pthread_create(&ui_thread_id, NULL, ui_thread, NULL));

    for (;;) {
        CHECK_ERRNO(pthread_mutex_lock(&local_mutex));
        while (socket_fd == -1) {
            CHECK_ERRNO(pthread_cond_wait(&no_socket, &local_mutex));
        }
        CHECK_ERRNO(pthread_mutex_unlock(&local_mutex));

        // Receive the package.
        uint8_t* temp_buffer = calloc(MAX_PSIZE, sizeof(uint8_t));
        ENSURE(temp_buffer != NULL);
        ssize_t bytes_read = receive_message(socket_fd, temp_buffer, MAX_PSIZE, MSG_DONTWAIT);

        if (bytes_read >= 0) {
            memcpy(&temp_session_id, temp_buffer, UINT64_T_SIZE);
            temp_session_id = be64toh(temp_session_id);
            memcpy(&received_byte_num, temp_buffer + UINT64_T_SIZE, UINT64_T_SIZE);
            received_byte_num = be64toh(received_byte_num);

            // If received session_id is bigger that the current one we reset the player.
            CHECK_ERRNO(pthread_mutex_lock(&mutex));
            if (temp_session_id > last_session_id) {
                is_playing = false;
                read_index = 0;
                buffer_fulfill = 0;
                last_session_id = temp_session_id;
                byte_0 = received_byte_num;

                packet_size = bytes_read - 2 * UINT64_T_SIZE;
                buffer_size = initial_buffer_size / packet_size * packet_size;

                memset(main_buffer, 0, buffer_size);
            }

            if (buffer_size >= packet_size) {
                bool is_empty = (buffer_fulfill == 0);
                memcpy(main_buffer + ((received_byte_num - byte_0) % buffer_size),
                    temp_buffer + 2 * UINT64_T_SIZE, packet_size);

                if (buffer_fulfill + packet_size < buffer_size) {
                    buffer_fulfill += packet_size;
                }
                if (is_empty) {
                    CHECK_ERRNO(pthread_cond_signal(&was_empty));
                }

                // If the condition from the task is satisfied let the thread write to output.
                if (received_byte_num >= byte_0 + (initial_buffer_size * 3 / 4)) {
                    is_playing = true;
                    CHECK_ERRNO(pthread_cond_signal(&play));
                }
            }
            // If the package size is bigger than size of buffer, don't process the package.
            else {
                fprintf(stderr, "Buffer size is smaller than packet size!\n");
            }
            CHECK_ERRNO(pthread_mutex_unlock(&mutex));
        }
        free(temp_buffer);
    }

    CHECK_ERRNO(pthread_join(writing_thread_id, NULL));
    CHECK_ERRNO(pthread_join(control_thread_sending_id, NULL));
    CHECK_ERRNO(pthread_join(ui_thread_id, NULL));
    CHECK_ERRNO(pthread_cond_destroy(&play));
    CHECK_ERRNO(pthread_cond_destroy(&was_empty));
    CHECK_ERRNO(pthread_cond_destroy(&no_socket));
    CHECK_ERRNO(pthread_mutex_destroy(&mutex));
    CHECK_ERRNO(pthread_mutex_destroy(&local_mutex));
    CHECK_ERRNO(pthread_mutex_destroy(&radio_list_mutex));
    CHECK_ERRNO(close(socket_fd));
    return 0;
} // RECEIVER == SERVER
