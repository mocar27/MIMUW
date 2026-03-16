#include <time.h>

#include "common.h"
#include "err.h"

char* host = NULL;
uint16_t data_port = DEFAULT_DATA_PORT;
char* data_port_str = "28612";
uint16_t control_port = DEFAULT_CONTROL_PORT;

int packet_size = DEFAULT_PSIZE;
uint64_t fifo_size = DEFAULT_FSIZE;
uint64_t r_time = DEFAULT_RTIME;
char* name = "Nienazwany Nadajnik";

atomic_bool is_sending = true;
atomic_int socket_fd = -1;

void* control_thread(void* data) {

    int control_socket = open_socket_udp();

    // Setting socket for sending and receiving control messages.
    struct ip_mreq ip_mreq;
    ip_mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    CHECK_ERRNO(inet_pton(AF_INET, host, &ip_mreq.imr_multiaddr));
    CHECK_ERRNO(setsockopt(control_socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (void*)&ip_mreq, sizeof ip_mreq));

    set_port_reuse(control_socket);
    bind_socket(control_socket, control_port);

    while (is_sending) {
        struct sockaddr_in sender_address;
        socklen_t sender_address_len = sizeof(sender_address);

        // Receive one of two possible control messages.
        char control_message[MAX_PSIZE];
        ssize_t received = recvfrom(control_socket, control_message, MAX_PSIZE, MSG_DONTWAIT,
            (struct sockaddr*)&sender_address, &sender_address_len);

        // Check whether received message is a lookup message.
        if (received >= 0 && is_sending) {
            if (strcmp(control_message, LOOKUP_MESSAGE) == 0) {
                char control_replay[MAX_PSIZE];
                ssize_t control_replay_len = snprintf(control_replay, MAX_PSIZE,
                    "%s %s %s %s\n", REPLAY_MESSAGE, host, data_port_str, name);
                ENSURE(control_replay_len >= 0);
                ENSURE(control_replay_len <= MAX_PSIZE);

                // Send REPLAY message to the sender of LOOKUP message.
                ssize_t sent = sendto(control_socket, control_replay, control_replay_len, NO_FLAGS,
                    (struct sockaddr*)&sender_address, sender_address_len);
                ENSURE(sent >= 0);
            }
        }
    }

    CHECK_ERRNO(setsockopt(control_socket, IPPROTO_IP, IP_DROP_MEMBERSHIP, (void*)&ip_mreq, sizeof(ip_mreq)));
    CHECK_ERRNO(close(control_socket));
    return 0;
}

int main(int argc, char** argv) {

    uint64_t session_id = (uint64_t)time(NULL);
    uint64_t first_byte_num = 0;

    int opt;
    opterr = 0;
    while ((opt = getopt(argc, argv, "a:P:C:p:f:R:n:")) != -1) {
        switch (opt) {
        case 'a':
            host = optarg;
            break;
        case 'P':
            data_port_str = optarg;
            data_port = read_port(optarg);
            break;
        case 'C':
            control_port = read_port(optarg);
            break;
        case 'p':
            packet_size = (int)atoi(optarg);
            break;
        case 'f':
            fifo_size = (uint64_t)strtoull(optarg, NULL, 10);
            break;
        case 'R':
            r_time = (uint64_t)strtoull(optarg, NULL, 10);
            break;
        case 'n':
            name = optarg;
            break;
        case '?':
            if (optopt == 'a' || optopt == 'P' || optopt == 'C' || optopt == 'p' ||
                optopt == 'f' || optopt == 'R' || optopt == 'n') {
                fatal("Option %s requires an argument!", argv[optind - 1]);
            }
        default:
            fatal("Invalid option or argument: %s", argv[optind - 1]);
        }
    }

    validate_arguments(host, name, r_time);
    is_multicast_address(host);
    if (packet_size < 1 || packet_size > MAX_PSIZE) {
        fatal("Packet size must be between 1 and 65507!");
    }
    if ((int)fifo_size < 0 || fifo_size > MAX_FSIZE) {
        fatal("FIFO size must be between 0 and UINT64_MAX!");
    }

    // Open socket to send messages and activate broadcast option.
    int socket_fd = open_socket_udp();
    int optval = 1;
    CHECK_ERRNO(setsockopt(socket_fd, SOL_SOCKET, SO_BROADCAST, (void*)&optval, sizeof optval));
    optval = 2;
    CHECK_ERRNO(setsockopt(socket_fd, IPPROTO_IP, IP_MULTICAST_TTL, (void*)&optval, sizeof optval));

    // Save remote multicast address and connect socket to it.
    struct sockaddr_in remote_address;
    remote_address.sin_family = AF_INET;
    remote_address.sin_port = htons(data_port);
    CHECK_ERRNO(inet_pton(AF_INET, host, &remote_address.sin_addr));
    connect_socket(socket_fd, &remote_address);

    // Create threads.
    pthread_t control_thread_id;
    CHECK_ERRNO(pthread_create(&control_thread_id, NULL, control_thread, NULL));

    // Array to read data from input and send session_id, first_byte_num and audio_data.
    uint8_t audio_data[packet_size + 2 * sizeof(uint64_t)];

    // Switch session_id to big-endian.
    uint64_t reverse_uint64 = htobe64(session_id);
    memcpy(audio_data, &reverse_uint64, sizeof(uint64_t));

    ssize_t bytes_read = readn(STDIN_FILENO, audio_data + 2 * UINT64_T_SIZE, packet_size);

    while (bytes_read == packet_size) {

        // Switch first_byte_num to big-endian.
        reverse_uint64 = htobe64(first_byte_num);
        memcpy(audio_data + UINT64_T_SIZE, &reverse_uint64, sizeof(uint64_t));

        // Send data to server.
        CHECK_ERRNO(send_message(socket_fd, audio_data, packet_size + 2 * UINT64_T_SIZE, NO_FLAGS));

        // Read data from input.
        bytes_read = readn(STDIN_FILENO, audio_data + 2 * UINT64_T_SIZE, packet_size);
        ENSURE(bytes_read >= 0);
        first_byte_num += (uint64_t)packet_size;
    }

    is_sending = false;
    CHECK_ERRNO(pthread_join(control_thread_id, NULL));
    CHECK_ERRNO(close(socket_fd));
    return 0;
} // SENDER == CLIENT
