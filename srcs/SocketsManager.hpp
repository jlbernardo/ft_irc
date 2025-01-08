#pragma once
#include <sys/select.h>

#include <map>
#include <queue>
#include <string>

class Server;
class Parser;

class SocketsManager {
public:
  SocketsManager(Server &server);
  ~SocketsManager();
  void add_new_sockets_from_masterset_to_read_write();
  void io_multiplexing();
  void load_client_queue(int fd);
  void socket_read(int fd);
  void broadcast_message(const Parser &message, int sender_fd);
  void socket_write(int fd);

private:
  fd_set _read_set;
  fd_set _write_set;
  std::map<int, std::queue<std::string> >
      _message_queues; // THIS IS GENERAL MESSAGE QUEUE, IT HOLD THE FORMATTED
                      // MESSAGE OF EVERY CLIENT
  Server &_server;
};
