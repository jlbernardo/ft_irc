#include "ft_irc.hpp"

void invite(Commands &commands, Command &cmd) {
    Client &sender = commands.get_sender();
	Server &server = sender.getServer();

    if (cmd.parameters.size() < 2) {
        server.send_message(sender.get_fd(), ERR_NEEDMOREPARAMS("INVITE"));
        return ;
    }

    std::string target_nickname = cmd.parameters[0];
    std::string channel_name = cmd.parameters[1];
    Channel *channel = server.get_channels()[channel_name];

    if (!server.channelExists(channel_name)) {
        server.send_message(sender.get_fd(), ERR_NOSUCHCHANNEL(channel_name));
        return ;
    }
    if (!server.clientExists(target_nickname)) {
        server.send_message(sender.get_fd(), ERR_NOSUCHNICK(target_nickname));
        return ;
    }
    if (!channel->isMember(&sender)) {
        server.send_message(sender.get_fd(), ERR_NOTONCHANNEL(channel_name));
        return ;
    }
    if (channel->isMember(&server.getClient(target_nickname))) {
        server.send_message(sender.get_fd(), ERR_USERONCHANNEL(target_nickname, channel_name));
        return ;
    }
    if (channel->mode('i') && !channel->isOperator(&sender)) {
        server.send_message(sender.get_fd(), ERR_CHANOPRIVSNEEDED(sender.get_nickname(), channel_name));
        return ;
    }

    Client &target = server.getClient(target_nickname);

    logger.debug("Inviting " + target.get_nickname() + " to " + channel_name);
    server.send_message(sender.get_fd(), RPL_INVITING(sender.get_nickname(), target.get_nickname(), channel_name));
    server.send_message(target.get_fd(), RPL_INVITE(sender.get_nickname(), target.get_nickname(), channel_name));

    channel->inviteMember(&target);
}