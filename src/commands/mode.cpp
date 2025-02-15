#include "ft_irc.hpp"


void mode(Commands &commands, const Command &cmd) {
    Client &client = commands.get_sender();
    Server &server = client.getServer();
    std::string channel = cmd.parameters[0];

    if (!server.checkForChannel(channel)) {
        server.send_message(client.get_fd(), ERR_NOSUCHCHANNEL(channel));
    }
    if (!server.get_channels()[channel]->isOperator(&client)) {
        server.send_message(client.get_fd(), ERR_CHANOPRIVSNEEDED(client.get_nickname(), cmd.parameters[1]));
    }

}