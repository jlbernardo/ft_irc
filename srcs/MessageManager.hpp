#pragma once
#include <sys/select.h>
class Server;

class MessageManager {
 public:
  MessageManager(Server &server, fd_set *read_set, fd_set *write_set);
  ~MessageManager() {}
  void populate_read_write_sets();
  void call_select(fd_set *read_set, fd_set *write_set);

 private:
  Server &_server;
  fd_set *_read_set;
  fd_set *_write_set;
};
