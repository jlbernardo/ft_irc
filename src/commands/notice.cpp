#include "ft_irc.hpp"


void notice(Commands &commands, Command &cmd) {
    Client &sender = commands.get_sender();
    Server &server = sender.getServer();
    
    if (!sender.is_authenticated() || cmd.parameters.empty() || cmd.parameters.size() < 2) {
        return;
    }

    std::string &recipient = cmd.parameters[0];
    std::string &message = cmd.parameters[1];

    if (recipient[0] == '#') {
        if (server.channelExists(recipient)) {
            Channel* channel = server.get_channels()[recipient];
            if (!channel->isMember(&sender)) 
                return;

            for (std::map<int, Client *>::const_iterator it = channel->getMembers().begin(); it != channel->getMembers().end(); ++it) {
                Client* member = it->second;
                if (member->get_fd() != sender.get_fd())
                    server.send_message(member->get_fd(), RPL_NOTICE(sender.get_nickname(), recipient, message));
            }
        }
    } else {
        for (std::map<int, Client *>::iterator it = server.get_clients().begin(); it != server.get_clients().end(); ++it) {
            Client* client = it->second;
            if (client->get_nickname() == recipient) {
                server.send_message(client->get_fd(), RPL_NOTICE(sender.get_nickname(), recipient, message));
                break;
            }
        }
    }
}