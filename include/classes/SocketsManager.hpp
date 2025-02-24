#pragma once
#ifndef SOCKETS_MANAGER_HPP
# define SOCKETS_MANAGER_HPP

# include "ft_irc.hpp"


class SocketsManager {
	private:
		fd_set _read_set;
		fd_set _write_set;
		Server &_server;

	public:
		~SocketsManager();
		SocketsManager(Server &server);
		SocketsManager(const SocketsManager &copy);
		SocketsManager &operator=(const SocketsManager &copy);

		void io_multiplexing();
		void socket_read(int fd);
		void socket_write(int fd);
		void load_client_queue(int fd);
		void add_new_sockets_from_masterset_to_read_write();
};

#endif
