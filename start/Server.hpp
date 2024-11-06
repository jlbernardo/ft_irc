#pragma once
#include <netinet/in.h>

#include <map>

#include "Client.hpp"
#include "Message.hpp"

class Server {
 private:
  int _server_fd;
  int _port;
  struct sockaddr_in _server_addr;
  fd_set _master_set;
  /* _max_fd is set according to the greatest fd number we have to handle, there is no fixed limit. */
  int _max_fd;
  std::map<int, Client*> _clients;

  void initialize_socket();
  void setup_server();
  void handle_new_connection();
  void handle_client_message(int client_fd);
  void broadcast_message(const Message& message, int sender_fd);
  void remove_client(int client_fd);

  // predicate functions:
  bool theres_data_in_this_fd(int fd, fd_set* read_set) const { return (true == FD_ISSET(fd, read_set)); }

 public:
  Server(int port);
  ~Server();

  void start();
  void stop();
};