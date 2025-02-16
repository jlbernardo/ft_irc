#include "ft_irc.hpp"


void mode(Commands &commands, const Command &cmd) {
    Client &client = commands.get_sender();
    Server &server = client.getServer();
    std::string target = cmd.parameters[0];

    std::map<std::string, Channel*> channels = server.get_channels();
    for (std::map<std::string, Channel*>::iterator it = channels.begin(); it != channels.end(); ++it) {
        logger.warn("Operator: " + it->second->getName());
    }
    if (target[0] == '#' || target[0] == '&') {


        if (!server.channelExists(target)) {
            server.send_message(client.get_fd(), ERR_NOSUCHCHANNEL(target));
            return ;
        }
        if (!server.get_channels()[target]->isOperator(&client)) {
            server.send_message(client.get_fd(), ERR_CHANOPRIVSNEEDED(client.get_nickname(), cmd.parameters[1]));
            return ;
        }
    }
}
