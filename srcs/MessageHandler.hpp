#pragma once
#include <sys/select.h>

#include "Server.hpp"

class Server;  // Forward declaration

class MessageHandler {
 public:
  MessageHandler(fd_set* read_set, fd_set* write_set, Server& server);
  void populate_sets();
  void call_select();
  void handle_messages();
  void read_client_messages(int fd);
  void send_client_messages(int fd);

 private:
  fd_set* read_set;
  fd_set* write_set;
  Server& server;
};
