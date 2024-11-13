#include <stdexcept>

#include "SocketsManager.hpp"

SocketsManager::SocketsManager(Server& serv) : server(serv) {}

SocketsManager::~SocketsManager() { message_queues.clear(); }

void SocketsManager::add_new_sockets_from_masterset_to_read_write() {
  FD_ZERO(&read_set);
  FD_ZERO(&write_set);
  if (server.max_fd < FD_SETSIZE) {
    for (int fd = 0; fd <= server.max_fd; fd++) {
      if (FD_ISSET(fd, &server.master_set)) {
        FD_SET(fd, &read_set);
        if (!message_queues[fd].empty()) {
          FD_SET(fd, &write_set);
        }
      }
    }
  }
}

void SocketsManager::io_multiplexing() {
  int result = select(server.max_fd + 1, &read_set, &write_set, NULL, NULL);
  if (result < 0) {
    if (server.terminate)
      throw std::runtime_error("Server received interrupt signal\n");
    else
      throw std::runtime_error("Select failed\n");
  }
}

void SocketsManager::socket_read(int fd) {
  if (FD_ISSET(fd, &read_set)) {
    if (fd == server.fd && server.max_fd < FD_SETSIZE)
      server.add_new_client_to_master_set();
    else
      load_client_queue(fd);
  }
}

void SocketsManager::load_client_queue(int client_fd) {
  Client& client = *server.clients[client_fd];
  if (!client.read_into_buffer()) {
    server.remove_client(client_fd);
    return;
  }
  if (client.buffer_has_linebreak()) {
    Message message(client, client.get_buffer());
    broadcast_message(message, client_fd);
    client.clean_buffer();
  }
}

void SocketsManager::broadcast_message(const Message& message, int sender_fd) {
  std::string formatted = message.format_message();
  for (std::map<int, Client*>::iterator it = server.clients.begin(); it != server.clients.end(); ++it) {
    int client_fd = it->first;
    if (client_fd != sender_fd) {
      message_queues[client_fd].push(formatted);
    }
  }
}

void SocketsManager::socket_write(int fd) {
  if (FD_ISSET(fd, &write_set) && !message_queues[fd].empty()) {
    while (!message_queues[fd].empty()) {
      std::string& msg = message_queues[fd].front();
      if (send(fd, msg.c_str(), msg.length(), 0) > 0)
        message_queues[fd].pop();
      else
        break;
    }
  }
}
