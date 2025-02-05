#include <stdexcept>

#include "Client.hpp"
#include "Commands.hpp"
#include "CommandManager.hpp"
#include "Server.hpp"
#include "SocketsManager.hpp"

SocketsManager::SocketsManager(Server& serv) : _server(serv) {}

SocketsManager::~SocketsManager() { _server._message_queues.clear(); }

void SocketsManager::add_new_sockets_from_masterset_to_read_write() {
  FD_ZERO(&_read_set);
  FD_ZERO(&_write_set);
  if (_server._max_fd < FD_SETSIZE) {
    for (int fd = 0; fd <= _server._max_fd; fd++) {
      if (FD_ISSET(fd, &_server._master_set)) {
        FD_SET(fd, &_read_set);
        if (!_server._message_queues[fd].empty()) {
          FD_SET(fd, &_write_set);
        }
      }
    }
  }
}

void SocketsManager::io_multiplexing() {
  int result = select(_server._max_fd + 1, &_read_set, &_write_set, NULL, NULL);
  if (result < 0) {
    if (_server.terminate)
      throw std::runtime_error("Server received interrupt signal\n");
    else
      throw std::runtime_error("Select failed\n");
  }
}

void SocketsManager::socket_read(int fd) {
  if (FD_ISSET(fd, &_read_set)) {
    if (fd == _server._fd && _server._max_fd < FD_SETSIZE)
      _server.add_new_client_to_master_set();
    else
      load_client_queue(fd);
  }
}

void SocketsManager::load_client_queue(int client_fd) {
  Client& client = *_server._clients[client_fd];
  if (!client.read_into_buffer()) {
    _server.remove_client(client_fd);
    return;
  }
  if (client.buffer_has_linebreak()) {
    Commands cmds(client, client.get_buffer());
    CommandManager cmd_manager(_server);
    cmd_manager.execute(cmds);
    if (cmds._fatal_error)
      _server.remove_client(client_fd);
    else
      client.clean_buffer();
  }
}

void SocketsManager::socket_write(int fd) {
  if (FD_ISSET(fd, &_write_set) && !_server._message_queues[fd].empty()) {
    while (!_server._message_queues[fd].empty()) {
      std::string& msg = _server._message_queues[fd].front();
      if (send(fd, msg.c_str(), msg.length(), 0) > 0)
        _server._message_queues[fd].pop();
      else
        break;
    }
  }
}
