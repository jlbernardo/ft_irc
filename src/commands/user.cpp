#include "ft_irc.h"


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
        sender.set_hostname(sender.get_fd());  // Pass the sender_fd as the socket descriptor
        sender.set_realname(realname);
        
        if (!sender.get_pass().empty() && !sender.get_nickname().empty()) {
            sender.set_authentication(true);

            server.send_message(sender.get_fd(), RPL_WELCOME(sender.get_username(), sender.get_identifier()));
            server.send_message(sender.get_fd(), RPL_YOURHOST(sender.get_nickname()));
            server.send_message(sender.get_fd(), RPL_CREATED(sender.get_nickname(), server.get_startup_date()));
            server.send_message(sender.get_fd(), RPL_MYINFO(sender.get_nickname(), "", ""));
        }
    }
}
