#pragma once
#ifndef SERVER_HPP
# define SERVER_HPP

# include "ft_irc.hpp"

typedef std::map<int, std::queue<std::string> > MessageQueueMap;

template <typename T> std::string to_string(T value) {
	std::ostringstream oss;
	oss << value;
	return oss.str();
}


class Server {
	private:
		int _fd;
		int _port;
		int _max_fd;
		std::string _pass;
		fd_set _master_set;
		std::string _startup_date;
		MessageQueueMap _message_queues;
		struct sockaddr_in _server_addr;
		std::map<int, Client *> _clients;
		std::map<std::string, Channel*> _channels;

		static void signal_handler(int signum);

		void setup_server();
		void register_signals();
		void initialize_socket();
		void add_new_client_to_master_set();
		void handle_client_message(int client_fd);

	public:
		static volatile __sig_atomic_t terminate;

		~Server();
		Server(const Server &copy);
		Server &operator=(const Server &copy);
		Server(int port, const std::string& pass);

		int get_fd();
		std::string &get_pass();
		std::string &get_startup_date();
		std::map<std::string, Channel*> &get_channels();
		std::map<int, Client *> &get_clients();
		MessageQueueMap &get_message_queues();
		void set_pass(const std::string& pass);
		void remove_client(int client_fd);

		void start();
		void send_error(int client_fd, const std::string& error_code, const std::string& error_message);
		void send_message(int client_fd, const std::string& message);
		void error(int fd, const std::string& msg);
		void addChannel(Channel* new_channel);
		bool channelExists(const std::string& channel_name);

		// Manager classes:
		friend class SocketsManager;
		friend class CommandsManager;
};

#endif
