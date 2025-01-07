#include "Client.hpp"
#include "ft_irc.h"
#include <iostream>

Client::Client(int fd) : fd(fd), nick("unknown"), user("unknown"), name("unknown"), authenticated(false), buffer("") {
  set_hostname(fd);
}

void Client::set_hostname(int client_socket) {
  struct sockaddr_in addr;
  socklen_t len = sizeof(addr);
  if (getsockname(client_socket, (struct sockaddr*)&addr, &len) == -1) {
    hostname = "unknown";
    println("getsockname has failed, thus the name is: ");
    printlnnl(hostname);
    return;
  }
  hostname = inet_ntoa(addr.sin_addr);
  println("The hostname will be ");
  printlnnl(hostname);
}

bool Client::read_into_buffer() {
  static char temp[1024];  // to automatically zero-initialize
  ssize_t bytes_read = recv(fd, temp, sizeof(temp) - 1, 0);
  if (bytes_read <= 0) {
    return false;
  }
  buffer.append(temp, bytes_read);
  println(buffer);
  return true;
}
