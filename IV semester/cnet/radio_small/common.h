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

#include "err.h"

#define DEFAULT_PORT 28612
#define DEFAULT_PSIZE 512
#define DEFAULT_BSIZE 65536
#define NO_FLAGS 0
#define UINT64_T_SIZE 8

uint16_t read_port(char* string) {
    errno = 0;
    if (string == NULL) {
        fatal("Port number not specified");
    }

    unsigned long port = strtoul(string, NULL, 10);
    PRINT_ERRNO();

    if (port > UINT16_MAX) {
        fatal("%lu is not a valid port number", port);
    }

    return (uint16_t)port;
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

int open_socket() {
    int socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    ENSURE(socket_fd >= 0);
    return socket_fd;
}

void bind_socket(int socket_fd, uint16_t port) {
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(port);

    CHECK_ERRNO(bind(socket_fd, (struct sockaddr*)&server_address,
        (socklen_t)sizeof(server_address)));
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
#endif // MIM_COMMON_H_
