#include "ft_irc.hpp"


void part(Commands &commands, Command &cmd) {
    Client &sender = commands.get_sender();
    Server &server = sender.getServer();

    if (cmd.parameters.empty()) {
        server.send_message(sender.get_fd(), ERR_NEEDMOREPARAMS("PART"));
        return ;
    }

    std::vector<std::string> channels = split(cmd.parameters[0], ',');
    std::string reason = cmd.parameters.size() > 1 ? cmd.parameters[1] : "";

    for (std::vector<std::string>::iterator it = channels.begin(); it != channels.end(); it++) {
        std::string channel_name = *it;
        
        if (!server.channelExists(channel_name)) {
            server.send_message(sender.get_fd(), ERR_NOSUCHCHANNEL(channel_name));
            continue ;
        }
        
        Channel *channel = server.get_channels()[channel_name];

        if (!channel->isMember(&sender)) {
            server.send_message(sender.get_fd(), ERR_NOTONCHANNEL(channel_name));
            continue ;
        }

        std::string message = RPL_PARTMSG(sender.get_identifier(), sender.get_username(), channel_name, reason);
        channel->broadcast(&sender, message);

        channel->removeMember(&sender);
        sender.remove_channel(channel);

        if (channel->isOperator(&sender))
            channel->removeOperator(&sender);

        if (channel->getCurrentMembersCount() == 0) {
            server.get_channels().erase(channel_name);
            delete channel;
        }

        if (channel->getCurrentOperatorsCount() == 0)
            channel->promoteFirstMember();
    }
}
