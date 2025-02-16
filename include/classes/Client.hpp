#pragma once
#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "ft_irc.hpp"


class Client {
	private:
		int _fd;
		std::string _nick;
		std::string _user;
		std::string _name;
		std::string _pass;
		std::string _buffer;
		std::string _hostname;
		std::string _identifier;

		Server &_server;
		bool _authenticated;
		std::vector<Channel*> _joinedChannels;

	public:
		~Client();
		Client(const Client &copy);
		Client(int fd,  Server &server);
		Client &operator=(const Client &copy);

		void clean_buffer();
		bool read_into_buffer();
		bool buffer_has_linebreak();

		// getters:
		int get_fd() const;
		Server &getServer() const;
		const std::string &get_pass() const;
		const std::string &get_buffer() const;
		const std::string &get_nickname() const;
		const std::string &get_username() const;
		const std::string &get_realname() const;
		const std::string &get_hostname() const;
		const std::string &get_identifier() const;
		const std::vector<Channel*> &get_joined_channels() const;

		// setters:
		void add_channel(Channel* channel);
		void set_hostname(int client_socket);
		void set_authentication(bool status);
		void set_password(const std::string &password);
		void set_nickname(const std::string &nickname);
		void set_username(const std::string &username);
		void set_realname(const std::string &realname);
		void set_identifier(const std::string &identifier);

		// checkers:
		bool is_authenticated() const;
		bool password_matched(Server &server) const;
};

#endif
