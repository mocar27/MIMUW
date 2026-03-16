// Patrz man basename, chcemy wersję GNU
#define _GNU_SOURCE

#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <arpa/inet.h>
#include <endian.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <netdb.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "err.h"

const int BUF_SIZE = 4096 * 128;

uint16_t
read_port(const char* string) {
  errno = 0;
  unsigned long port = strtoul(string, NULL, 10);
  PRINT_ERRNO();
  if (port > UINT16_MAX) {
    fatal("%u is not a valid port number", port);
  }

  return (uint16_t)port;
}

struct sockaddr_in
  get_send_address(const char* host, uint16_t port) {
  struct addrinfo hints;
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_INET; // IPv4
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_protocol = IPPROTO_UDP;

  struct addrinfo* address_result;
  CHECK(getaddrinfo(host, NULL, &hints, &address_result));

  struct sockaddr_in send_address;
  send_address.sin_family = AF_INET; // IPv4
  send_address.sin_addr.s_addr =
    ((struct sockaddr_in*)(address_result->ai_addr))
    ->sin_addr.s_addr;               // IP address
  send_address.sin_port = htons(port); // port from the command line

  freeaddrinfo(address_result);

  return send_address;
}

ssize_t
readn(int fd, void* buf, size_t n) {
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

ssize_t
writen(int fd, const void* buf, size_t n) {
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

int
main(int argc, char* argv[]) {
  if (argc != 4) {
    fatal("Usage: file-client-tcp host port file_name");
  }

  const char* host = argv[1];
  const char* port_str = argv[2];
  const char* file_name = argv[3];

  uint16_t port = read_port(port_str);
  struct sockaddr_in addr = get_send_address(host, port);

  int fd;
  if ((fd = open(file_name, O_RDONLY)) == -1)
    PRINT_ERRNO();
  const char* name = basename(file_name);
  size_t name_len = strlen(name);
  if (name_len == 0) {
    fatal("Will not send a directory.");
  }
  if (name_len > USHRT_MAX) {
    fatal("File name too long.");
  }
  struct stat file_info;
  CHECK_ERRNO(fstat(fd, &file_info));
  off_t size = file_info.st_size;

  int sock = socket(PF_INET, SOCK_STREAM, 0);
  if (sock == -1) PRINT_ERRNO();
  CHECK_ERRNO(connect(sock, &addr, sizeof addr));
  uint32_t size_n = htonl((uint32_t)size);
  CHECK_ERRNO(writen(sock, &size_n, 4));
  uint16_t name_len_n = htons((uint16_t)name_len);
  CHECK_ERRNO(writen(sock, &name_len_n, 2));
  CHECK_ERRNO(writen(sock, name, name_len));

  while (size) {
    uint8_t buf[BUF_SIZE];
    size_t len = (size_t)size < sizeof buf ? (size_t)size : sizeof buf;
    ssize_t n = readn(fd, buf, len);
    if (n == -1)
      PRINT_ERRNO();
    CHECK_ERRNO(writen(sock, buf, n));
    size -= n;
  }

  CHECK_ERRNO(close(sock));
  CHECK_ERRNO(close(fd));

  return 0;
}
