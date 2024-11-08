#include "MessageManager.hpp"
#include "Server.hpp"

MessageManager::MessageManager(Server &server, fd_set *read_set, fd_set *write_set)
    : _server(server), _read_set(read_set), _write_set(write_set) {}

void MessageManager::populate_read_write_sets() {
  FD_ZERO(_read_set);
  FD_ZERO(_write_set);
  for (int fd = 0; fd <= _server._max_fd; fd++) {
    if (_server._max_fd < FD_SETSIZE) {
      if (FD_ISSET(fd, &_server._master_set)) {
        FD_SET(fd, _read_set);
        if (!_server._clients[fd]->queue.empty()) {
          FD_SET(fd, _write_set);
        }
      }
    }
  }
}

void MessageManager::call_select(fd_set *read_set, fd_set *write_set) {
  if (select(_server._max_fd + 1, read_set, write_set, NULL, NULL) < 0) {
    if (_server._terminate)
      throw std::runtime_error("Server Received Interrupt Signal\n");
    else
      throw std::runtime_error("Select failed\n");
  }
}
