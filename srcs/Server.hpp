#pragma once
#include <netinet/in.h>

#include <csignal>
#include <map>

#include "Client.hpp"
#include "Message.hpp"

class Server {
 private:
  int fd;
  int port;
  fd_set master_set;
  int max_fd;
  std::map<int, Client*> clients;
  struct sockaddr_in server_addr;
  static void signal_handler(int signum);

  void register_signals();
  void initialize_socket();
  void setup_server();
  void add_new_client_to_master_set();
  void handle_client_message(int client_fd);
  void broadcast_message(const Message& message, int sender_fd);
  void remove_client(int client_fd);

 public:
  Server(int port);
  ~Server();
  static volatile sig_atomic_t terminate;
  friend class SocketsManager;

  void start();
  void stop();
};
