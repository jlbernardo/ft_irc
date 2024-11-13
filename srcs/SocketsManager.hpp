#pragma once
#include <sys/select.h>

#include <queue>

#include "Server.hpp"

class Server;

class SocketsManager {
 public:
  SocketsManager(fd_set* read_set, fd_set* write_set, Server& server);
  ~SocketsManager();
  void add_new_sockets_from_masterset_to_read_write();
  void io_multiplexing();
  void load_client_queue(int fd);
  void socket_read(int fd);
  void broadcast_message(const Message& message, int sender_fd);
  void socket_write(int fd);

 private:
  fd_set* read_set;
  fd_set* write_set;
  std::map<int, std::queue<std::string> > message_queues;
  Server& server;
};
