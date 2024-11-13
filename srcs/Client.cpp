#include "Client.hpp"

bool Client::read_into_buffer() {
  static char temp[1024];  // to automatically zero-initialize
  ssize_t bytes_read = recv(fd, temp, sizeof(temp) - 1, 0);
  if (bytes_read <= 0) {
    return false;
  }
  buffer.append(temp, bytes_read);
  return true;
}
