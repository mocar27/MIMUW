#ifndef MIM_COMMON_H_
#define MIM_COMMON_H_

#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <endian.h>

#include <string.h>
#include <unistd.h>
#include <stdint.h>

#include <stdatomic.h>
#include <pthread.h>

#include "err.h"

#define DEFAULT_DISCOVER_ADDR "255.255.255.255"
#define DEFAULT_UI_PORT 18612
#define DEFAULT_DATA_PORT 28612
#define DEFAULT_CONTROL_PORT 38612
#define DEFAULT_PSIZE 512
#define DEFAULT_BSIZE 65536
#define DEFAULT_FSIZE 131072
#define DEFAULT_RTIME 250

#define MAX_PSIZE 65507
#define MAX_BSIZE UINT64_MAX
#define MAX_FSIZE UINT64_MAX
#define MAX_PORT_LEN 5
#define MAX_NAME_LENGTH 64
#define MIN_ASCII 32
#define MAX_ASCII 127

#define NO_FLAGS 0
#define TTL_VALUE 2
#define UINT64_T_SIZE 8

#define LOOKUP_MESSAGE "ZERO_SEVEN_COME_IN\n"
#define LOOKUP_MESSAGE_SIZE 19
#define REPLAY_MESSAGE "BOREWICZ_HERE"
#define REPLAY_MESSAGE_SIZE 14
#define LOOKUP_SLEEP_TIME 5

#define QUEUE_LENGTH 5
#define UP_ARROW 65
#define DOWN_ARROW 66
#define UI_LINE "------------------------------------------------------------------------"
#define UI_NAME " SIK Radio"
#define INITIAL_UI_SIZE 1024

uint16_t read_port(char* string) {
    errno = 0;
    if (string == NULL) {
        fatal("Port number not specified");
    }

    for (char* c = string; *c != '\0'; c++) {
        if (*c < '0' || *c > '9') {
            fatal("Port number must be a number");
        }
    }

    unsigned long port = strtoul(string, NULL, 10);
    PRINT_ERRNO();

    if (port > UINT16_MAX || port == 0) {
        fatal("%lu is not a valid port number", port);
    }

    return (uint16_t)port;
}

void set_port_reuse(int socket_fd) {
    int option_value = 1;
    CHECK_ERRNO(setsockopt(socket_fd, SOL_SOCKET, SO_REUSEPORT, &option_value, sizeof(option_value)));
}

struct sockaddr_in get_address(char* host, uint16_t port) {
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;

    struct addrinfo* result;
    CHECK(getaddrinfo(host, NULL, &hints, &result));

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = ((struct sockaddr_in*)result->ai_addr)->sin_addr.s_addr;

    freeaddrinfo(result);
    return address;
}

int open_socket_udp() {
    int socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    ENSURE(socket_fd >= 0);
    return socket_fd;
}

int open_socket_tcp() {
    int socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    ENSURE(socket_fd >= 0);
    return socket_fd;
}

int accept_connection(int socket_fd, struct sockaddr_in* client_address) {
    socklen_t client_address_length = (socklen_t)sizeof(*client_address);

    int client_fd = accept(socket_fd, (struct sockaddr*)client_address, &client_address_length);
    if (client_fd < 0) {
        PRINT_ERRNO();
    }

    return client_fd;
}

void start_listening(int socket_fd, size_t queue_length) {
    CHECK_ERRNO(listen(socket_fd, queue_length));
}

void bind_socket(int socket_fd, uint16_t port) {
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(port);

    CHECK_ERRNO(bind(socket_fd, (struct sockaddr*)&server_address,
        (socklen_t)sizeof(server_address)));
}

void connect_socket(int socket_fd, const struct sockaddr_in* address) {
    CHECK_ERRNO(connect(socket_fd, (struct sockaddr*)address, sizeof(*address)));
}

ssize_t readn(int socket_fd, void* buffer, size_t n) {
    char* buf = buffer;
    size_t left = n;

    while (left) {
        ssize_t result = read(socket_fd, buf, left);
        if (result == -1) {
            if (errno == EINTR) {
                continue;
            }
            return -1;
        }
        if (result == 0) {
            break;
        }
        left -= result;
        buf += result;
    }

    return n - left;
}

ssize_t writen(int socket_fd, void* buffer, size_t n) {
    char* buf = buffer;
    size_t left = n;

    while (left) {
        ssize_t result = write(socket_fd, buf, left);
        if (result == -1) {
            if (errno == EINTR) {
                continue;
            }
            return -1;
        }
        left -= result;
        buf += result;
    }

    return n;
}

void send_message(int socket_fd, const void* message, size_t length, int flags) {
    errno = 0;
    ssize_t sent_length = send(socket_fd, message, length, flags);
    if (sent_length < 0) {
        PRINT_ERRNO();
    }
    ENSURE(sent_length == (ssize_t)length);
}

size_t receive_message(int socket_fd, void* buffer, size_t max_length, int flags) {
    errno = 0;
    ssize_t received_length = recv(socket_fd, buffer, max_length, flags);
    // Not handling -1 return value here, because main handles
    // it in a special way -- it process package if retured value by receive_message
    // is not -1, else it will later wait on conditional variable.
    return (size_t)received_length;
}

void is_multicast_address(char* host) {
    struct sockaddr_in addr;
    inet_pton(AF_INET, host, &(addr.sin_addr));

    if (!(IN_MULTICAST(ntohl(addr.sin_addr.s_addr)))) {
        fatal("Address is not a multicast address");
    }
}

void validate_arguments(char* host, char* sender_name, uint64_t raport_time) {
    if (host == NULL) {
        fatal("IP address not specified. '-a' parameter is required");
    }

    if (sender_name != NULL) {
        size_t name_length = strlen(sender_name);
        if (name_length == 0 || name_length > MAX_NAME_LENGTH) {
            fatal("Name length must be between 1 and 64!");
        }
        for (size_t i = 0; i < name_length; i++) {
            if (i == 0 && sender_name[i] == ' ') {
                fatal("Name cannot start with a space!");
            }
            if (i == name_length - 1 && sender_name[i] == ' ') {
                fatal("Name cannot end with a space!");
            }
            if (sender_name[i] < MIN_ASCII || sender_name[i] > MAX_ASCII) {
                fatal("Name must contain only printable characters!");
            }
        }
    }
    if ((int)raport_time < 1) {
        fatal("Raport time must be greater than 0!");
    }
    get_address(host, 0);
}

bool parse_message_arguments(char* message, char** host, char** data_port_str, char** station_name) {
    int message_it = REPLAY_MESSAGE_SIZE;

    if (message[message_it] == ' ') {
        return false;
    }

    char addr[INET_ADDRSTRLEN];
    memset(addr, 0, INET_ADDRSTRLEN);
    int it = 0;
    while ((message_it <= strlen(message) && message[message_it] != ' ' && it < INET_ADDRSTRLEN)) {
        if ((message[message_it] < '0' || message[message_it] > '9') && message[message_it] != '.') {
            return false;
        }

        addr[it] = message[message_it];
        message_it++;
        it++;
    }

    if (message[message_it] != ' ' || (message[message_it] == ' ' && message[message_it + 1] == ' ')) {
        return false;
    }

    char port[MAX_PORT_LEN + 1];
    memset(port, 0, MAX_PORT_LEN + 1);
    it = 0;
    message_it++;
    while (message_it <= strlen(message) && message[message_it] != ' ' && it < MAX_PORT_LEN) {
        if (message[message_it] < '0' || message[message_it] > '9') {
            return false;
        }

        port[it] = message[message_it];
        message_it++;
        it++;
    }

    if (message[message_it] != ' ' || (message[message_it] == ' ' && message[message_it + 1] == ' ')) {
        return false;
    }

    char name[MAX_NAME_LENGTH + 1];
    memset(name, 0, MAX_NAME_LENGTH + 1);
    it = 0;
    message_it++;
    while (message_it <= strlen(message) && message[message_it] != '\n' && it < MAX_NAME_LENGTH) {
        name[it] = message[message_it];
        message_it++;
        it++;
    }

    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;

    struct addrinfo* result;
    int a = getaddrinfo(addr, NULL, &hints, &result);
    freeaddrinfo(result);
    if (a != 0) {
        return false;
    }

    struct sockaddr_in temp_addr;
    inet_pton(AF_INET, addr, &(temp_addr.sin_addr));

    if (!(IN_MULTICAST(ntohl(temp_addr.sin_addr.s_addr)))) {
        return false;
    }

    errno = 0;
    unsigned long p = strtoul(port, NULL, 10);
    if (errno != 0 || p > UINT16_MAX || p == 0) {
        return false;
    }

    strcpy(*host, addr);
    strcpy(*data_port_str, port);
    strcpy(*station_name, name);

    return true;
}

#endif // MIM_COMMON_H_
