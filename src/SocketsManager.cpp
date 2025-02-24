#include "ft_irc.hpp"

SocketsManager::SocketsManager(Server& serv) : _server(serv) {
}

SocketsManager::~SocketsManager() {
  _server._message_queues.clear();
}

SocketsManager::SocketsManager(const SocketsManager& copy) : _server(copy._server) {
  *this = copy;
}

SocketsManager& SocketsManager::operator=(const SocketsManager& copy) {
  if (this != &copy)
    _server = copy._server;
  return *this;
}

void SocketsManager::add_new_sockets_from_masterset_to_read_write() {
  FD_ZERO(&_read_set);
  FD_ZERO(&_write_set);

  if (_server._max_fd > FD_SETSIZE)
    return;

  for (int fd = 0; fd <= _server._max_fd; fd++) {
    if (!FD_ISSET(fd, &_server._master_set))
      continue;

    FD_SET(fd, &_read_set);

    if (!_server._message_queues[fd].empty())
      FD_SET(fd, &_write_set);
  }
}

void SocketsManager::io_multiplexing() {
  int result = select(_server._max_fd + 1, &_read_set, &_write_set, NULL, NULL);

  if (result >= 0)
    return ;

  if (result < 0 && _server.terminate)
    throw std::runtime_error("Server received interrupt signal");
  
  throw std::runtime_error("Select failed");
}

void SocketsManager::socket_read(int fd) {
  if (!FD_ISSET(fd, &_read_set))
    return ;

  if (fd == _server._fd && _server._max_fd < FD_SETSIZE)
    _server.add_new_client_to_master_set();
  else
    load_client_queue(fd);
}

void SocketsManager::load_client_queue(int client_fd) {
  Client& client = *_server._clients[client_fd];

  if (!client.read_into_buffer()) {
    _server.remove_client(client_fd);
    return;
  }

  if (client.buffer_has_linebreak()) {
    Commands cmds(client, client.get_buffer());
    CommandsManager cmd_manager(_server);

    cmd_manager.execute(cmds);

    if (cmds.get_fatal_error())
      _server.remove_client(client_fd);
    else if (_server._clients.find(client_fd) != _server._clients.end())
      client.clean_buffer();
  }
}

void SocketsManager::socket_write(int fd) {
  if (FD_ISSET(fd, &_write_set) && !_server._message_queues[fd].empty()) {
    while (!_server._message_queues[fd].empty()) {
      std::string& msg = _server._message_queues[fd].front();
      
      if (send(fd, msg.c_str(), msg.length(), 0) <= 0)
        break;

      _server._message_queues[fd].pop();
    }
  }
}
