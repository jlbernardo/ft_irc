#pragma once
#ifndef SOCKETS_MANAGER_HPP
# define SOCKETS_MANAGER_HPP

# include "ft_irc.hpp"


class SocketsManager {
public:
  SocketsManager(Server &server);
  ~SocketsManager();
  void add_new_sockets_from_masterset_to_read_write();
  void io_multiplexing();
  void load_client_queue(int fd);
  void socket_read(int fd);
  void socket_write(int fd);

private:
  fd_set _read_set;
  fd_set _write_set;
  Server &_server;
};

#endif
