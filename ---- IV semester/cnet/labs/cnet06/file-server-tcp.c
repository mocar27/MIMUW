#include <sys/types.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include "err.h"
#include "common.h"

#define FILE_BUFFER 4096 * 128
#define QUEUE_LENGTH 5

size_t total_size;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

ssize_t readn(int fd, void* buf, size_t n) {
    char* b = buf;
    size_t left = n;

    while (left) {
        ssize_t res = read(fd, b, left);
        if (res == -1) {
            if (errno == EINTR)
                continue;
            return -1;
        }
        else if (res == 0) {
            break;
        }
        left -= res;
        b += res;
    }
    return n - left;
}

ssize_t writen(int fd, const void* buf, size_t n) {
    const char* b = buf;
    size_t left = n;

    while (left) {
        ssize_t res = write(fd, b, left);
        if (res == -1) {
            if (errno == EINTR)
                continue;
            return -1;
        }
        left -= res;
        b += res;
    }
    return n;
}

void* handle_connection(void* client_fd_ptr) {
    int client_fd = *(int*)client_fd_ptr;
    free(client_fd_ptr);

    char* ip = get_ip_from_socket(client_fd);
    int port = get_port_from_socket(client_fd);

    printf("[thread %lu, pid %d] %s:%d connected\n", (unsigned long)pthread_self(), getpid(), ip, port);

    int file_length = 0;
    short file_name_length = 0;
    char file_name[FILE_BUFFER];

    ssize_t read = readn(client_fd, &file_length, sizeof(file_name));

    if (read != 4) {
        printf("error reading file length\n");
        return 0;
    }
    else {
        file_length = ntohl(file_length);
    }

    read = readn(client_fd, &file_name_length, sizeof(file_name_length));
    if (read != 2) {
        printf("error reading file name length\n");
        return 0;
    }
    else {
        file_name_length = ntohs(file_name_length);
    }

    read = readn(client_fd, file_name, file_name_length);
    if (read != file_name_length) {
        printf("error reading file name\n");
        return 0;
    }

    int file_descriptor = open(file_name, O_WRONLY | O_CREAT);
    if (file_descriptor == -1) {
        printf("error opening file\n");
        return 0;
    }

    sleep(1);

    char buffer[FILE_BUFFER];
    int temp = file_length;;

    while (temp > 0) {
        read = readn(client_fd, buffer, FILE_BUFFER);
        if (read == -1) {
            printf("error reading file\n");
            return 0;
        }
        else {
            temp -= read;
            CHECK_ERRNO(writen(file_descriptor, buffer, read));
        }
    }

    printf("[thread %lu, pid %d] --> %.*s\n", (unsigned long)pthread_self(), getpid(), (int)read, file_name);
    printf("client [%s:%d] has sent its file of size=[%d]\n", ip, port, file_length);

    CHECK_ERRNO(pthread_mutex_lock(&mutex));
    total_size += file_length;
    printf("total size of uploaded files [%lu]\n", total_size);
    CHECK_ERRNO(pthread_mutex_unlock(&mutex));

    printf("[thread %lu, pid %d] connection closed\n", (unsigned long)pthread_self(), getpid());
    CHECK_ERRNO(close(file_descriptor));
    CHECK_ERRNO(close(client_fd));
    return 0;
}

int main(int argc, char* argv[]) {
    int socket_fd = open_socket();

    // pthread_mutex_t mutex;
    // CHECK_ERRNO(pthread_mutex_init(&mutex, NULL));

    int port = read_port(argv[1]);
    printf("Listening on port %d\n", port);

    start_listening(socket_fd, QUEUE_LENGTH);

    for (;;) {
        struct sockaddr_in client_addr;

        int client_fd = accept_connection(socket_fd, &client_addr);

        // Arguments for the thread must be passed by pointer
        int* client_fd_pointer = malloc(sizeof(int));
        if (client_fd_pointer == NULL) {
            fatal("malloc");
        }
        *client_fd_pointer = client_fd;

        pthread_t thread;
        CHECK_ERRNO(pthread_create(&thread, 0, handle_connection, client_fd_pointer));
        CHECK_ERRNO(pthread_detach(thread));
    }
    printf("total size of uploaded files [%lu]\n", total_size);
    CHECK_ERRNO(pthread_mutex_destroy(&mutex));
}

