#include "ft_irc.hpp"


void join(Commands &commands, const Command &cmd) {
    Channel* channel;
    Client &sender = commands.get_sender();
    Server &server = sender.getServer();
    std::string channel_name = cmd.parameters[0];

    if (server.channelExists(channel_name)) {
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

    Channel* created_channel = new Channel(channel_name, &sender);
    server.addChannel(created_channel);
    sender.add_channel(created_channel);
    channel = created_channel;

    logger.debug("CHANNEL CREATION INFO");
    logger.debug("Channel name: " + channel->getName());
    logger.debug("Channel topic: " + channel->getTopic());
    logger.debug("Channel members count: " + to_string(channel->getCurrentMembersCount()));
    
    std::map<std::string, Channel*> server_ch = server.get_channels();

    for (std::map<std::string, Channel*>::iterator it = server_ch.begin(); it != server_ch.end(); ++it) {
        logger.warn("Server channel list: " + it->second->getName());
        logger.warn("Channel is operator: " + it->second->_operators.begin()->second->get_nickname());
    }

    if (server.get_channels().find(channel_name) != server.get_channels().end()) {
        logger.info("Channel created successfully: " + channel_name);
    }
    else {
       logger.warn("Failed to create channel: " + channel_name);
       return ;
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