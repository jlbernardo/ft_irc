#include <arpa/inet.h>
#include <unistd.h>

#include <csignal>
#include <cstring>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include "MessageHandler.hpp"
#include "Server.hpp"

volatile sig_atomic_t Server::terminate = 0;

void Server::register_signals() {
  int signals[6] = {SIGINT, SIGTERM, SIGQUIT, SIGHUP, SIGPIPE, SIGABRT};
  for (size_t i = 0; i < 6; ++i) {
    signal(signals[i], signal_handler);
  }
}

Server::Server(int port) : port(port), max_fd(0) {
  register_signals();
  initialize_socket();
  setup_server();
}

void Server::signal_handler(int signum) { terminate = signum; }

void Server::initialize_socket() {
  fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd < 0) {
    throw std::runtime_error("Failed to create socket\n");
  }
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(port);
}

void Server::start() {
  fd_set read_set;
  fd_set write_set;
  MessageHandler handler(&read_set, &write_set, *this);

  while (true) {
    if (max_fd < FD_SETSIZE) {
      handler.populate_sets();
    }
    handler.call_select();
    handler.handle_messages();
  }
}

void Server::setup_server() {
  if (bind(fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
    close(fd);
    throw std::runtime_error("Failed to bind socket\n");
  } else if (listen(fd, SOMAXCONN) < 0) {
    close(fd);
    throw std::runtime_error("Failed to listen on socket\n");
  }
  FD_ZERO(&master_set);
  FD_SET(fd, &master_set);
  max_fd = fd;
}

void Server::add_new_client_to_master_set() {
  struct sockaddr_in client_addr;
  socklen_t client_addr_len = sizeof(client_addr);

  int client_fd = accept(fd, (struct sockaddr *)&client_addr, &client_addr_len);
  if (client_fd < 0) {
    std::cerr << "Failed to accept connection\n" << std::endl;
    return;
  }

  FD_SET(client_fd, &master_set);
  if (client_fd > max_fd) max_fd = client_fd;
  clients[client_fd] = new Client(client_fd);
  std::stringstream str;
  str << "Connected to server on socket " << client_fd;
  send(client_fd, str.str().c_str(), str.str().length(), 0);
}

void Server::remove_client(int client_fd) {
  FD_CLR(client_fd, &master_set);
  close(client_fd);
  delete clients[client_fd];
  clients.erase(client_fd);
}

Server::~Server() {
  std::cout << "\nShutting down server\n";
  for (std::map<int, Client *>::iterator it = clients.begin(); it != clients.end(); ++it) {
    delete it->second;
    close(it->first);
  }
  close(fd);
}
