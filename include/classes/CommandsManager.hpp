#pragma once
#ifndef COMMANDS_MANAGER_HPP
# define COMMANDS_MANAGER_HPP

# include "ft_irc.hpp"


class CommandsManager {
	public:
		~CommandsManager();
		CommandsManager(Server &server);
		CommandsManager(const CommandsManager &copy);
		CommandsManager &operator=(const CommandsManager &copy);

		void execute(Commands &commands);

	private:
		Server &server;

		void send_welcome_messages(Client &client);
		bool is_nickname_in_use(const std::string &new_nick);
		void broadcast_message(const std::string &msg, int sender_fd);
		void update_nickname(Client &client, const std::string &new_nick);
		void update_user_info(Client &client, const std::string &username, const std::string &realname);
		void broadcast_nickname_change(int sender_fd, const std::string &old_nick, const std::string &new_nick);
};

#endif
