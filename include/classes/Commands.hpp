#pragma once
#ifndef COMMANDS_HPP
# define COMMANDS_HPP

# include "ft_irc.hpp"

enum CommandType { PRIVMSG, JOIN, NICK, USER, QUIT, KICK,  PASS, INVITE, TOPIC, MODE, WHO, UNKNOWN };

struct Command {
	std::string command;
	std::string params;
	CommandType type;
	std::vector<std::string> parameters;
};


class Commands {
	private:
		int _sender_fd;
		Client &_sender;
		bool _fatal_error;
		std::list<Command> _list;
		std::map<std::string, CommandType> _command_map;

		void init_command_map();
		void to_uppercase(std::string &str);
		void init_list_of_commands(const std::string &raw_input);
		CommandType find_command_type(const std::string &command);
		std::vector<std::string> separate_multiple_commands(const std::string &input, const std::string &delimiter);

	public:
		~Commands();
		Commands(const Commands &copy);
		Commands &operator=(const Commands &copy);
		Commands(Client &sender, const std::string &raw_input);

		Client &get_sender() const;
		std::list<Command> &get_list();
		void set_fatal_error(bool fatal_error) { _fatal_error = fatal_error; }
		bool get_fatal_error() const { return _fatal_error; }
		void clear() { _list.clear(); }
};

#endif
