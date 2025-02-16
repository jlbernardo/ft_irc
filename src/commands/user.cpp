#include "ft_irc.hpp"


void user(Commands &commands, const Command &cmd) {
    const std::vector<std::string>& params = cmd.parameters;
    Client &sender = commands.get_sender();
    Server &server = sender.getServer();

    // Debug print to verify parameters
    logger.info("Parameters size: " + to_string(params.size()));

    if (params.size() < 4) {
        server.send_message(sender.get_fd(), ERR_NEEDMOREPARAMS(cmd.command));
    }
    else if (sender.is_authenticated()) {
        server.send_message(sender.get_fd(), ERR_ALREADYREGISTERED(sender.get_username()));
    }
    else {
        std::string username = params[0];
        std::string realname = params[3];

        sender.set_username(username);
        sender.set_hostname(sender.get_fd());
        sender.set_realname(realname);
        
        if (!sender.get_pass().empty() && !sender.get_nickname().empty()) {
            std::string nick = sender.get_nickname();
            sender.set_authentication(true);

            sender.set_identifier(nick + "!" + sender.get_username() + "@" + sender.get_hostname());

            server.send_message(sender.get_fd(), RPL_WELCOME(sender.get_username(), sender.get_identifier()));
            server.send_message(sender.get_fd(), RPL_YOURHOST(nick));
            server.send_message(sender.get_fd(), RPL_CREATED(nick, server.get_startup_date()));
            server.send_message(sender.get_fd(), RPL_MYINFO(nick, "", ""));
        }
    }
}
