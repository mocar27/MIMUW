#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <netinet/ip_icmp.h>

#include "err.h"

#define BSIZE 1000
#define ICMP_HEADER_LEN 8
#define NO_FLAGS 0

bool is_program_interrupted = false;
int icmp_seq_send = 0;

unsigned short in_cksum(unsigned short* addr, int len);
void drop_to_nobody();

void install_signal_handler(int signal, void (*handler)(int), int flags) {
    struct sigaction action;
    sigset_t block_mask;

    sigemptyset(&block_mask);
    action.sa_handler = handler;
    action.sa_mask = block_mask;
    action.sa_flags = flags;

    CHECK_ERRNO(sigaction(signal, &action, NULL));
}

void catch_sigint(int sig) {
    is_program_interrupted = true;
}

void send_ping(int sock, char* s_send_addr) {
    struct addrinfo addr_hints;
    struct addrinfo* addr_result;
    memset(&addr_hints, 0, sizeof(struct addrinfo));
    addr_hints.ai_family = AF_INET;
    addr_hints.ai_socktype = SOCK_RAW;
    addr_hints.ai_protocol = IPPROTO_ICMP;
    CHECK(getaddrinfo(s_send_addr, 0, &addr_hints, &addr_result));

    struct sockaddr_in send_addr;
    send_addr.sin_family = AF_INET;
    send_addr.sin_addr.s_addr = ((struct sockaddr_in*)(addr_result->ai_addr))->sin_addr.s_addr;
    send_addr.sin_port = htons(0);
    freeaddrinfo(addr_result);

    struct icmp* icmp;
    char send_buffer[BSIZE];
    memset(send_buffer, 0, sizeof(send_buffer));
    icmp = (struct icmp*)send_buffer;
    icmp->icmp_type = ICMP_ECHO;
    icmp->icmp_code = 0;
    icmp->icmp_id = htons(getpid());
    icmp->icmp_seq = htons(icmp_seq_send++);

    struct timeval timeval;
    gettimeofday(&timeval, 0);
    long long t = timeval.tv_sec * 1000000 + timeval.tv_usec;

    ssize_t data_len = snprintf(((char*)send_buffer + ICMP_HEADER_LEN), sizeof(send_buffer) - ICMP_HEADER_LEN, "%lld", t);
    ENSURE(data_len > 0);

    ssize_t icmp_len = data_len + ICMP_HEADER_LEN;
    icmp->icmp_cksum = 0;
    icmp->icmp_cksum = in_cksum((unsigned short*)icmp, icmp_len);

    ssize_t len = sendto(sock, (void*)icmp, icmp_len, 0, (struct sockaddr*)&send_addr, (socklen_t)sizeof(send_addr));
    ENSURE(icmp_len == len);
}

bool receive_ping(int sock) {
    struct sockaddr_in rcv_addr;
    char rcv_buffer[BSIZE];
    memset(rcv_buffer, 0, sizeof(rcv_buffer));
    socklen_t rcv_addr_len = (socklen_t)sizeof(rcv_addr);
    ssize_t len = recvfrom(sock, (void*)rcv_buffer, sizeof(rcv_buffer), 0, (struct sockaddr*)&rcv_addr, &rcv_addr_len);
    ENSURE(len >= 0);

    struct ip* ip;
    ip = (struct ip*)rcv_buffer;
    ssize_t ip_header_len = ip->ip_hl << 2;

    struct icmp* icmp;
    icmp = (struct icmp*)(rcv_buffer + ip_header_len);
    ssize_t icmp_len = len - ip_header_len;

    printf("%zd bytes from %s ", len, inet_ntoa(rcv_addr.sin_addr));
    printf("icmp_seq=%d ttl=%d ", ntohs(icmp->icmp_seq), ip->ip_ttl);

    ENSURE(icmp_len >= ICMP_HEADER_LEN);
    ENSURE(icmp->icmp_type == ICMP_ECHOREPLY);
    ENSURE(ntohs(icmp->icmp_id) == getpid());

    ssize_t time_index = ip_header_len + ICMP_HEADER_LEN;
    long long sending_package_time = strtoll(rcv_buffer + time_index, NULL, 10);

    struct timeval timeval;
    gettimeofday(&timeval, 0);
    long long t = timeval.tv_sec * 1000000 + timeval.tv_usec;

    double time = (double)(t - sending_package_time) / (double)1000;

    printf("time=%.3f ms\n", time);

    return true;
}

int main(int argc, char** argv) {

    if (argc < 2) {
        fatal("Usage: %s host\n", argv[0]);
    }

    int sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    ENSURE(sock >= 0);

    install_signal_handler(SIGINT, catch_sigint, NO_FLAGS);
    drop_to_nobody();

    while (!is_program_interrupted) {
        send_ping(sock, argv[1]);

        while (!receive_ping(sock)) {
        }
        sleep(1);
    }

    CHECK_ERRNO(close(sock));
    return 0;
}