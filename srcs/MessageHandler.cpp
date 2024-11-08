#include <unistd.h>

#include <stdexcept>

#include "MessageHandler.hpp"

MessageHandler::MessageHandler(fd_set* read, fd_set* write, Server& serv)
    : read_set(read), write_set(write), server(serv) {}

void MessageHandler::populate_sets() {
  FD_ZERO(read_set);
  FD_ZERO(write_set);
  for (int fd = 0; fd <= server.max_fd; fd++) {
    if (FD_ISSET(fd, &server.master_set)) {
      FD_SET(fd, read_set);
      if (!server.message_queues[fd].empty()) {
        FD_SET(fd, write_set);
      }
    }
  }
}

void MessageHandler::call_select() {
  int result = select(server.max_fd + 1, read_set, write_set, NULL, NULL);
  if (result < 0) {
    if (server.terminate)
      throw std::runtime_error("Server Received Interrupt Signal\n");
    else
      throw std::runtime_error("Select failed\n");
  }
}

void MessageHandler::handle_messages() {
  for (int fd = 0; fd <= server.max_fd; fd++) {
    read_client_messages(fd);
    send_client_messages(fd);
  }
}

void MessageHandler::read_client_messages(int fd) {
  if (FD_ISSET(fd, read_set)) {
    if (fd == server.fd && server.max_fd < FD_SETSIZE)
      server.add_new_client_to_master_set();
    else
      server.handle_client_message(fd);
  }
}

void MessageHandler::send_client_messages(int fd) {
  if (FD_ISSET(fd, write_set) && !server.message_queues[fd].empty()) {
    while (!server.message_queues[fd].empty()) {
      std::string& msg = server.message_queues[fd].front();
      if (send(fd, msg.c_str(), msg.length(), 0) > 0)
        server.message_queues[fd].pop();
      else
        break;
    }
  }
}
