#include <time.h>

#include "common.h"
#include "err.h"

char* host = NULL;
uint16_t port = DEFAULT_PORT;
uint64_t packet_size = DEFAULT_PSIZE;
char* name = "Nienazwany Nadajnik";

int main(int argc, char** argv) {

    uint64_t session_id = (uint64_t)time(NULL);
    uint64_t first_byte_num = 0;

    int opt;
    opterr = 0;
    while ((opt = getopt(argc, argv, "a:P:p:n:")) != -1) {
        switch (opt) {
        case 'a':
            host = optarg;
            break;
        case 'P':
            port = read_port(optarg);
            break;
        case 'p':
            packet_size = (uint64_t)atoi(optarg);
            break;
        case 'n':
            name = optarg;
            break;
        case '?':
            if (optopt == 'a' || optopt == 'P' || optopt == 'p' || optopt == 'n') {
                fatal("Option %s requires an argument!", argv[optind - 1]);
            }
        default:
            fatal("Invalid option or argument: %s", argv[optind - 1]);
        }
    }
    if (host == NULL) { 
        fatal("Host address not specified"); 
    }

    // Get server adress and open socket to send messages.
    struct sockaddr_in server_address = get_address(host, port);
    int socket_fd = open_socket();

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
        ssize_t bytes_sent = sendto(socket_fd, &audio_data, sizeof(audio_data), NO_FLAGS,
            (struct sockaddr*)&server_address, sizeof(server_address));
        ENSURE(bytes_sent >= 0);

        // Read data from input.
        bytes_read = readn(STDIN_FILENO, audio_data + 2 * UINT64_T_SIZE, packet_size);
        ENSURE(bytes_read >= 0);
        first_byte_num += packet_size;
    }

    CHECK_ERRNO(close(socket_fd));
    return 0;
} // SENDER == CLIENT
