#include "ft_irc.hpp"

void topic(Commands &commands, Command &cmd) {
    Client &client = commands.get_sender();
    Server &server = client.getServer();

    if (cmd.parameters.size() == 0) {
        server.send_message(client.get_fd(), ERR_NEEDMOREPARAMS("TOPIC"));
        return ;
    }

    std::string channel_name = cmd.parameters.front();
    cmd.parameters.erase(cmd.parameters.begin());

    if (!server.channelExists(channel_name)) {
        server.send_message(client.get_fd(), ERR_NOSUCHCHANNEL(channel_name));
        return ;
    }

    Channel *channel = server.get_channels()[channel_name];

    if (!channel->isMember(&client)) {
        server.send_message(client.get_fd(), ERR_NOTONCHANNEL(channel->getName()));
        return ;
    }

    if (cmd.parameters.size() == 0) {
        if (channel->getTopic().empty())
            server.send_message(client.get_fd(), RPL_NOTOPIC(client.get_nickname(), channel->getName()));
        else
            server.send_message(client.get_fd(), RPL_TOPIC(client.get_nickname(), channel->getName(), channel->getTopic()));
        return ;
    }

    if (channel->mode('t') && !channel->isOperator(&client)) {
        server.send_message(client.get_fd(), ERR_CHANOPRIVSNEEDED(client.get_nickname(), channel->getName()));
        return ;
    }

    std::string topic = cmd.parameters.front();
    cmd.parameters.erase(cmd.parameters.begin());

    channel->setTopic(topic);
    channel->broadcast(&client, RPL_TOPICCHANGE(client.get_nickname(), channel->getName(), topic));
}
