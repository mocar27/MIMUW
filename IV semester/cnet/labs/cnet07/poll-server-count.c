/*
 Ten program używa poll(), aby równocześnie obsługiwać wielu klientów
 bez tworzenia procesów ani wątków.
*/

#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>

#include "common.h"
#include "err.h"

#define BUF_SIZE 1024
#define QUEUE_LENGTH 5
#define TIMEOUT 5000
#define CONNECTIONS 10

size_t total_clients;
size_t current_clients;
static bool finish = false;

/* Obsługa sygnału kończenia */
static void catch_int(int sig) {
    finish = true;
    fprintf(stderr,
        "Signal %d catched. No new connections will be accepted.\n", sig);
}

int main(int argc, char* argv[]) {

    if (argc != 3) {
        fatal("Usage: %s port control_port\n", argv[0]);
    }

    char buf[BUF_SIZE];

    install_signal_handler(SIGINT, catch_int, SA_RESTART);

    struct pollfd poll_descriptors[CONNECTIONS];
    /* Inicjujemy tablicę z gniazdkami klientów, poll_descriptors[0] to gniazdko centrali */
    for (int i = 0; i < CONNECTIONS; ++i) {
        poll_descriptors[i].fd = -1;
        poll_descriptors[i].events = POLLIN | POLLOUT;
        poll_descriptors[i].revents = 0;
    }

    /* Tworzymy gniazdko centrali */
    uint16_t port = read_port(argv[1]);
    poll_descriptors[0].fd = open_socket();
    bind_socket(poll_descriptors[0].fd, port);

    uint16_t control_port = read_port(argv[2]);
    poll_descriptors[1].fd = open_socket();
    bind_socket(poll_descriptors[1].fd, control_port);

    printf("Listening on port %u\n", port);
    printf("Listening on control port %u\n", control_port);

    start_listening(poll_descriptors[0].fd, QUEUE_LENGTH);
    start_listening(poll_descriptors[1].fd, QUEUE_LENGTH);

    do {
        for (int i = 0; i < CONNECTIONS; ++i) {
            poll_descriptors[i].revents = 0;
        }

        /* Po Ctrl-C zamykamy gniazdko centrali */
        if (finish && poll_descriptors[0].fd >= 0) {
            CHECK_ERRNO(close(poll_descriptors[0].fd));
            poll_descriptors[0].fd = -1;
        }

        if (finish && poll_descriptors[1].fd >= 0) {
            CHECK_ERRNO(close(poll_descriptors[1].fd));
            poll_descriptors[1].fd = -1;
        }

        int poll_status = poll(poll_descriptors, CONNECTIONS, TIMEOUT);
        if (poll_status == -1) {
            if (errno == EINTR)
                fprintf(stderr, "Interrupted system call\n");
            else
                PRINT_ERRNO();
        }
        else if (poll_status > 0) {
            if (!finish && (poll_descriptors[0].revents & POLLIN)) {
                /* Przyjmuję nowe połączenie */
                int client_fd = accept_connection(poll_descriptors[0].fd, NULL);

                bool accepted = false;
                for (int i = 3; i < CONNECTIONS; ++i) {
                    if (poll_descriptors[i].fd == -1) {
                        fprintf(stderr, "Received new connection (%d)\n", i);
                        current_clients++;
                        total_clients++;
                        poll_descriptors[i].fd = client_fd;
                        poll_descriptors[i].events = POLLIN;
                        accepted = true;
                        break;
                    }
                }
                if (!accepted) {
                    CHECK_ERRNO(close(client_fd));
                    fprintf(stderr, "Too many clients\n");
                }
            }

            if (!finish && (poll_descriptors[1].revents & POLLIN)) {
                int client_fd = accept_connection(poll_descriptors[1].fd, NULL);
                fprintf(stderr, "Received new control connection\n");
                poll_descriptors[2].fd = client_fd;
                poll_descriptors[2].events = POLLIN;
            }

            if (poll_descriptors[2].fd != -1 && (poll_descriptors[2].revents & (POLLIN | POLLERR))) {
                ssize_t received_bytes = read(poll_descriptors[2].fd, buf, BUF_SIZE);
                if (received_bytes < 0) {
                    fprintf(stderr, "Error when reading message from control connection (errno %d, %s)\n", errno, strerror(errno));
                }
                else if (received_bytes == 0) {
                    fprintf(stderr, "Ending control connection");
                }
                else {
                    if (buf[0] != 'c') {
                        fprintf(stderr, "Wrong command, ignored and disconnecting\n");
                    }
                    else {
                        fprintf(stderr, "Sending control clients info\n");
                        char control_buffer[100];
                        sprintf(control_buffer, "Number of active clients: %lu\nTotal number of clients: %lu\n", current_clients, total_clients);
                        ssize_t bytes_send = send(poll_descriptors[2].fd, control_buffer, strlen(control_buffer), NO_FLAGS);
                        ENSURE(bytes_send > 0);
                    }
                }
                CHECK_ERRNO(close(poll_descriptors[2].fd));
                poll_descriptors[2].fd = -1;
            }

            for (int i = 3; i < CONNECTIONS; ++i) {
                if (poll_descriptors[i].fd != -1 && (poll_descriptors[i].revents & (POLLIN | POLLERR))) {
                    ssize_t received_bytes = read(poll_descriptors[i].fd, buf, BUF_SIZE);
                    if (received_bytes < 0) {
                        fprintf(stderr, "Error when reading message from connection %d (errno %d, %s)\n", i, errno, strerror(errno));
                        CHECK_ERRNO(close(poll_descriptors[i].fd));
                        poll_descriptors[i].fd = -1;
                        current_clients--;
                    }
                    else if (received_bytes == 0) {
                        fprintf(stderr, "Ending connection (%d)\n", i);
                        current_clients--;
                        CHECK_ERRNO(close(poll_descriptors[i].fd));
                        poll_descriptors[i].fd = -1;
                    }
                    else {
                        printf("(%d) -->%.*s\n", i, (int)received_bytes, buf);
                    }
                }
            }
        }
        else {
            printf("%d milliseconds passed without any events\n", TIMEOUT);
        }
    } while (!finish || current_clients > 0);

    if (poll_descriptors[0].fd >= 0)
        CHECK_ERRNO(close(poll_descriptors[0].fd));

    if (poll_descriptors[1].fd >= 0)
        CHECK_ERRNO(close(poll_descriptors[1].fd));

    exit(EXIT_SUCCESS);
}
