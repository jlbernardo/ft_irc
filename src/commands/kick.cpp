#include "ft_irc.hpp"

void kick(Commands &commands, Command &cmd) {
    Client &sender = commands.get_sender();
    Server &server = sender.getServer();

    if (cmd.parameters.size() < 2) {
        server.send_message(sender.get_fd(), ERR_NEEDMOREPARAMS("KICK"));
        return ;
    }

    std::string channel_name = cmd.parameters[0];
    std::vector<std::string> users = split(cmd.parameters[1], ',');
    std::string reason = cmd.parameters.size() > 2 ? cmd.parameters[2] : "just cause";

    if (!server.channelExists(channel_name)) {
        server.send_message(sender.get_fd(), ERR_NOSUCHCHANNEL(channel_name));
        return ;
    }
    if (!server.get_channels()[channel_name]->isMember(&sender)) {
        server.send_message(sender.get_fd(), ERR_NOTONCHANNEL(channel_name));
        return ;
    }
    if (!server.get_channels()[channel_name]->isOperator(&sender)) {
        server.send_message(sender.get_fd(), ERR_CHANOPRIVSNEEDED(sender.get_nickname(), channel_name));
        return ;
    }

    Channel *channel = server.get_channels()[channel_name];

    for (std::vector<std::string>::iterator it = users.begin(); it != users.end(); it++) {
        std::string target_name = *it;

        if (!server.clientExists(target_name)) {
            server.send_message(sender.get_fd(), ERR_NOSUCHNICK(target_name));
            continue ;
        }

        Client &target = server.getClient(target_name);

        if (!channel->isMember(&target)) {
            server.send_message(sender.get_fd(), ERR_USERNOTINCHANNEL(sender.get_nickname(), target_name, channel_name));
            continue ;
        }

        std::string message = RPL_KICKREASON(sender.get_identifier(), channel_name, target.get_nickname(), reason);
        channel->broadcast(&sender, message);
        server.send_message(sender.get_fd(), message);

        channel->removeMember(&target);
        target.remove_channel(channel);
    }
}
