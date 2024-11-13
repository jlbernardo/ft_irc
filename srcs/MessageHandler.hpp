#pragma once
#include <sys/select.h>

#include <queue>

#include "Server.hpp"

class Server;  // Forward declaration

class MessageHandler {
 public:
  MessageHandler(fd_set* read_set, fd_set* write_set, Server& server);
  ~MessageHandler();
  void populate_sets();
  void call_select();
  void handle_messages();
  void monitor_client_messages(int fd);
  void send_messages_to_client(int fd);
  void read_client_messages(int fd);
  void enqueue_message(const Message& message, int sender_fd);

 private:
  fd_set* read_set;
  fd_set* write_set;
  std::map<int, std::queue<std::string> > message_queues;
  Server& server;
};
