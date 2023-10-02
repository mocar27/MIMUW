#include <stdatomic.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "common.h"
#include "err.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t play = PTHREAD_COND_INITIALIZER;
atomic_bool is_playing = false;

pthread_cond_t was_empty = PTHREAD_COND_INITIALIZER;
atomic_ullong buffer_fulfill = 0;
atomic_ullong read_index = 0;

char* client_ip = NULL;
struct sockaddr_in client;
uint16_t port = DEFAULT_PORT;
uint64_t buffer_size = DEFAULT_BSIZE;
atomic_ullong packet_size = 0;

uint64_t last_session_id = 0;
uint64_t byte_0 = 0;

void* writing_thread(void* data) {

    uint8_t* buffer = data;

    for (;;) {

        CHECK_ERRNO(pthread_mutex_lock(&mutex));
        while (!is_playing) {
            CHECK_ERRNO(pthread_cond_wait(&play, &mutex));
        }
        while (buffer_fulfill == 0) {
            CHECK_ERRNO(pthread_cond_wait(&was_empty, &mutex));
        }

        ssize_t bytes_written = writen(STDOUT_FILENO, buffer + read_index, packet_size);
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

int main(int argc, char** argv) {

    int opt;
    opterr = 0;
    while ((opt = getopt(argc, argv, "a:P:b:")) != -1) {
        switch (opt) {
        case 'a':
            client_ip = optarg;
            break;
        case 'P':
            port = read_port(optarg);
            break;
        case 'b':
            buffer_size = (uint64_t)atoi(optarg);
            break;
        case '?':
            if (optopt == 'a' || optopt == 'P' || optopt == 'b') {
                fatal("Option %s requires an argument!", argv[optind - 1]);
            }
        default:
            fatal("Invalid option or argument: %s", argv[optind - 1]);
        }
    }

    if (client_ip == NULL) {
        fatal("Client address not specified");
    }

    int socket_fd = open_socket();
    bind_socket(socket_fd, port);

    struct sockaddr_in receiving_address;
    socklen_t receiving_address_len;
    receiving_address_len = sizeof(receiving_address);

    // Main buffer for both threads, music data is here.
    uint8_t buffer[buffer_size];
    memset(buffer, 0, buffer_size);
    uint64_t initial_buffer_size = buffer_size;

    // tablica otrzymanych paczek - jakie paczki przyszly a jakie nie (indeksy)
    bool received_packets[buffer_size];
    memset(received_packets, false, buffer_size);

    // Values session_id and first_byte_num received in one package.
    uint64_t temp_session_id = 0;
    uint64_t received_byte_num = 0;

    pthread_t writing_thread_id;
    CHECK_ERRNO(pthread_create(&writing_thread_id, NULL, writing_thread, buffer));

    for (;;) {

        // Receive the package.
        uint8_t* temp_buffer = calloc(DEFAULT_BSIZE, sizeof(uint8_t));
        ssize_t bytes_read = recvfrom(socket_fd, temp_buffer, DEFAULT_BSIZE, NO_FLAGS,
            (struct sockaddr*)&receiving_address, &receiving_address_len);
        ENSURE(bytes_read >= 0);

        memcpy(&temp_session_id, temp_buffer, UINT64_T_SIZE);
        temp_session_id = be64toh(temp_session_id);
        memcpy(&received_byte_num, temp_buffer + UINT64_T_SIZE, UINT64_T_SIZE);
        received_byte_num = be64toh(received_byte_num);

        // If IP of client from received package doesn't match we don't process the package.
        if (receiving_address.sin_addr.s_addr == inet_addr(client_ip)) {

            // If received session_id is bigger that the current one we reset the player.
            CHECK_ERRNO(pthread_mutex_lock(&mutex));
            if (temp_session_id > last_session_id) {

                is_playing = false;
                read_index = 0;

                last_session_id = temp_session_id;
                byte_0 = received_byte_num;

                packet_size = bytes_read - 2 * UINT64_T_SIZE;
                buffer_size = buffer_size / packet_size * packet_size;

                memset(buffer, 0, buffer_size);
                memset(received_packets, false, buffer_size);
            }

            if (buffer_size >= packet_size) {

                bool is_empty = (buffer_fulfill == 0);
                memcpy(buffer + ((received_byte_num - byte_0) % buffer_size),
                    temp_buffer + 2 * UINT64_T_SIZE, packet_size);
                received_packets[(received_byte_num - byte_0) % buffer_size] = true;

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
    CHECK_ERRNO(pthread_cond_destroy(&play));
    CHECK_ERRNO(pthread_cond_destroy(&was_empty));
    CHECK_ERRNO(pthread_mutex_destroy(&mutex));
    CHECK_ERRNO(close(socket_fd));
    return 0;
} // RECEIVER == SERVER
