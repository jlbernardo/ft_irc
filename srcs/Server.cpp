#include <arpa/inet.h>
#include <unistd.h>

#include <csignal>
#include <cstring>
#include <iostream>
#include <stdexcept>

#include "Server.hpp"

volatile sig_atomic_t Server::_terminate = 0;

Server::Server(int port) : _port(port), _max_fd(0) {
  signal(SIGINT, signal_handler);
  initialize_socket();
  setup_server();
}

void Server::signal_handler(int signum) { _terminate = signum; }

void Server::initialize_socket() {
  _server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (_server_fd < 0) {
    throw std::runtime_error("Failed to create socket\n");
  }
  memset(&_server_addr, 0, sizeof(_server_addr));
  _server_addr.sin_family = AF_INET;
  _server_addr.sin_addr.s_addr = INADDR_ANY;
  _server_addr.sin_port = htons(_port);
}

void Server::broadcast_message(const Message &message, int sender_fd) {
  std::string formatted = message.format_message();
  for (std::map<int, Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
    int client_fd = it->first;
    if (client_fd != sender_fd) {
      _message_queues[client_fd].push(formatted);
    }
  }
}

// Modify the start method in Server.cpp:
void Server::start() {
  while (!_terminate) {
    fd_set read_set;
    fd_set write_set;
    FD_ZERO(&read_set);
    FD_ZERO(&write_set);

    // Populate read_set and write_set
    for (int fd = 0; fd <= _max_fd; fd++) {
      if (FD_ISSET(fd, &_master_set)) {
        FD_SET(fd, &read_set);
        if (!_message_queues[fd].empty()) {
          FD_SET(fd, &write_set);
        }
      }
    }

    if (select(_max_fd + 1, &read_set, &write_set, NULL, NULL) < 0) {
      if (_terminate)
        return;
      else
        throw std::runtime_error("Select failed\n");
    }

    for (int fd = 0; fd <= _max_fd; fd++) {
      if (theres_data_in_this_fd(fd, &read_set)) {
        if (fd == _server_fd)
          add_new_client_to_master_set();
        else
          handle_client_message(fd);
      }
      if (FD_ISSET(fd, &write_set) && !_message_queues[fd].empty()) {
        while (_message_queues[fd].size() > 0) {
          std::string &msg = _message_queues[fd].front();
          if (send(fd, msg.c_str(), msg.length(), 0) > 0) _message_queues[fd].pop();
        }
      }
    }
  }
}

void Server::setup_server() {
  if (bind(_server_fd, (struct sockaddr *)&_server_addr, sizeof(_server_addr)) < 0) {
    close(_server_fd);
    throw std::runtime_error("Failed to bind socket\n");
  } else if (listen(_server_fd, SOMAXCONN) < 0) {
    close(_server_fd);
    throw std::runtime_error("Failed to listen on socket\n");
  }
  FD_ZERO(&_master_set);
  FD_SET(_server_fd, &_master_set);
  _max_fd = _server_fd;
}

void Server::add_new_client_to_master_set() {
  struct sockaddr_in client_addr;
  socklen_t client_addr_len = sizeof(client_addr);

  int client_fd = accept(_server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
  if (client_fd < 0) {
    std::cerr << "Failed to accept connection\n" << std::endl;
    return;
  }

  FD_SET(client_fd, &_master_set);
  if (client_fd > _max_fd) _max_fd = client_fd;
  _clients[client_fd] = new Client(client_fd);
  std::cout << "\rNew connection on socket " << client_fd << "     " << std::flush;
}

void Server::handle_client_message(int client_fd) {
  char buffer[1024] = {0};
  ssize_t bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);
  if (bytes_read <= 0) {
    remove_client(client_fd);
    return;
  }
  Message message(client_fd, buffer);
  broadcast_message(message, client_fd);
}

void Server::remove_client(int client_fd) {
  FD_CLR(client_fd, &_master_set);
  close(client_fd);
  delete _clients[client_fd];
  _message_queues.erase(client_fd);
  _clients.erase(client_fd);
}

Server::~Server() {
  std::cout << "\nShutting down server\n";
  for (std::map<int, Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
    delete it->second;
    _message_queues.erase(it->first);
    close(it->first);
  }
  _message_queues.clear();
  close(_server_fd);
}
