#include "ft_irc.h"


void mode(Commands &commands, const Command &cmd) {
    Client &client = commands.get_sender();
    Server &server = client.getServer();
    
    if (!server.checkForChannel(cmd.parameters[1])) {
        server.send_message(client.get_fd(), ERR_NOSUCHCHANNEL(cmd.parameters[1]));
    }
    if (!server.get_channels()[cmd.parameters[1]]->isOperator(&client)) {
        server.send_message(client.get_fd(), ERR_CHANOPRIVSNEEDED(client.get_nickname(), cmd.parameters[1]));
    }

}