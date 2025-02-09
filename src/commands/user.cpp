#include "ft_irc.h"


void user(Commands &commands, const Command &cmd) {
    const std::vector<std::string>& params = cmd.parameters;
    Client &client = commands.get_sender();
    Server &server = client.getServer();

    // Debug print to verify parameters
    logger.info("Parameters size: " + params.size());

    if (params.size() < 4) {
        server.send_message(client.get_fd(), ERR_NEEDMOREPARAMS(cmd.command));
    }
    else if (client.is_authenticated()) {
        server.send_message(client.get_fd(), ERR_ALREADYREGISTERED(client.get_username()));
    }
    else {
        std::string username = params[0];
        std::string realname = params[3];

        client.set_username(username);
        client.set_hostname(client.get_fd());  // Pass the sender_fd as the socket descriptor
        client.set_realname(realname);
        
        server.send_message(client.get_fd(), RPL_WELCOME( client.get_username(), client.get_identifier()));
        server.send_message(client.get_fd(), RPL_YOURHOST(client.get_nickname()));
        server.send_message(client.get_fd(), RPL_CREATED(client.get_nickname()));
        server.send_message(client.get_fd(), RPL_MYINFO(client.get_nickname(), "", ""));
    }
}
