#include "ft_irc.hpp"

void who(Commands &commands, const Command &cmd) {
    Client &client = commands.get_sender();
    Server &server = client.getServer();
    std::string mask = cmd.parameters[0];

    if (mask.empty()) {
        server.send_message(client.get_fd(), ERR_NEEDMOREPARAMS("WHO"));
        return ;
    }

    logger.debug("WHO: " + mask);
    if (mask[0] == '#' || mask[0] == '&') {
        if (!server.channelExists(mask)) {
            server.send_message(client.get_fd(), ERR_NOSUCHCHANNEL(mask));
            return ;
        }

        Channel *channel = server.get_channels()[mask];
        logger.debug("CHANNEL NAME: " + channel->getName());

        for (std::map<int, Client *>::const_iterator it = channel->getMembers().begin(); it != channel->getMembers().end(); ++it) {
            Client *member = it->second;

            std::string username = member->get_username();
            std::string hostname = member->get_hostname();
            std::string nickname = member->get_nickname();
            std::string realname = member->get_realname();

            server.send_message(client.get_fd(), RPL_WHOREPLY(mask, username, hostname, nickname, "H", realname));
        }
    }
    else {
        std::map<int, Client *> clients = server.get_clients();
        Client *target = NULL;

        for (std::map<int, Client *>::const_iterator it = clients.begin(); it != clients.end(); ++it) {
            Client &client = *it->second;
            if (client.get_nickname() == mask) {
                target = &client;
                break ;
            }
            if (it == clients.end()) {
                server.send_message(client.get_fd(), ERR_NOSUCHNICK(mask));
                return ;
            }
        }
        
        std::string username = target->get_username();
        std::string hostname = target->get_hostname();
        std::string nickname = target->get_nickname();
        std::string realname = target->get_realname();

        server.send_message(client.get_fd(), RPL_WHOREPLY(mask, username, hostname, nickname, "H", realname));
    }

    server.send_message(client.get_fd(), RPL_ENDOFWHO(mask));
}
