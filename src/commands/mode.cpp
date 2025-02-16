#include "ft_irc.hpp"


void mode(Commands &commands, const Command &cmd) {
    Client &client = commands.get_sender();
    Server &server = client.getServer();
    std::string target = cmd.parameters[0];

    if (target[0] == '#' || target[0] == '&') {

        if (!server.channelExists(target)) {
            server.send_message(client.get_fd(), ERR_NOSUCHCHANNEL(target));
            return ;
        }

        if (cmd.parameters.size() == 1) {
            // Channel *channel = server.get_channels()[target];
            // server.send_message(client.get_fd(), RPL_CHANNELMODEIS(target, channel->getMode(), channel->getModeParams()));
            return ;
        }
        else if (cmd.parameters.size() == 2) {
            server.send_message(client.get_fd(), ERR_NEEDMOREPARAMS("MODE"));
            return ;
        }
        else {
            if (!server.get_channels()[target]->isOperator(&client)) {
                server.send_message(client.get_fd(), ERR_CHANOPRIVSNEEDED(client.get_nickname(), cmd.parameters[1]));
                return ;
            }
        }

    }
    else {

    }
}
