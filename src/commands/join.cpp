#include "ft_irc.hpp"


void join(Commands &commands, const Command &cmd) {
    Channel* channel;
    Client &sender = commands.get_sender();
    Server &server = sender.getServer();
    std::string channel_name = cmd.parameters[0];

    if (server.checkForChannel(channel_name)) {
        channel = server.get_channels()[channel_name];
        if (!channel->isOperator(&sender)) {
            if (channel->mode('l') && channel->getCurrentMembersCount() < channel->getUserLimit()) {
                channel->addMember(&sender);
                server.send_message(sender.get_fd(), RPL_JOIN(sender.get_identifier(), channel_name));
                if (!channel->getTopic().empty()) {
                    server.send_message(sender.get_fd(), RPL_TOPIC(sender.get_identifier(), channel_name, channel->getTopic()));
                }
            }
            else if (channel->mode('l')) {
                server.send_message(sender.get_fd(), ERR_CHANNELISFULL(channel_name));
            }
            else {
                channel->addMember(&sender);
            }
        }
        return ;
    }

    Channel created_channel(channel_name, &sender);
    channel = &created_channel;

    if (server.get_channels().find(channel_name) != server.get_channels().end()) {
       logger.info("Channel created successfully: " + channel_name);
    }
    else {
       logger.warn("Failed to create channel: " + channel_name);
    }
    std::string names;

    for (std::map<int, Client *>::const_iterator it = channel->getMembers().begin(); it != channel->getMembers().end(); ++it) {
        Client* member = it->second;
        if (member != &sender) {
            server.send_message(member->get_fd(), RPL_JOIN(sender.get_identifier(), channel_name));
            if (channel->isOperator(member))
                names += "@";
            names += member->get_nickname() + " ";
        }
    }

    names += "@" + sender.get_nickname();
    server.send_message(sender.get_fd(), RPL_JOIN(sender.get_username(), channel_name));
    if (!channel->getTopic().empty()) {
        server.send_message(sender.get_fd(), RPL_TOPIC(sender.get_nickname(), channel_name, channel->getTopic()));
    }
    server.send_message(sender.get_fd(), RPL_NAMREPLY(sender.get_nickname(), channel_name, names));
    server.send_message(sender.get_fd(), RPL_ENDOFNAMES(sender.get_nickname(), channel_name)); 
}