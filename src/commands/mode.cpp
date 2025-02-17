#include "ft_irc.hpp"


void mode(Commands &commands, Command &cmd) {
    Client &client = commands.get_sender();
    Server &server = client.getServer();
    std::string target = cmd.parameters.front();
    cmd.parameters.erase(cmd.parameters.begin());

    if (target[0] == '#' || target[0] == '&') {

        if (!server.channelExists(target)) {
            server.send_message(client.get_fd(), ERR_NOSUCHCHANNEL(target));
            return ;
        }

        Channel *channel = server.get_channels()[target];

        if (cmd.parameters.size() == 0) {
            server.send_message(client.get_fd(), RPL_CHANNELMODEIS(target, channel->getModes(), channel->getModeParams()));
            return ;
        }

        if (!channel->isOperator(&client)) {
            server.send_message(client.get_fd(), ERR_CHANOPRIVSNEEDED(client.get_username(), target));
            return ;
        }

        std::string mode = cmd.parameters.front();
        cmd.parameters.erase(cmd.parameters.begin());

        for (size_t i = 1; i < mode.size(); i++) {
            switch (mode[i]) {
                case 'i':
                    channel->setInviteOnly(mode[0] == '+' ? true : false);
                    break;
                case 't':
                    channel->setTopicRestricted(mode[0] == '+' ? true : false);
                    break;
                case 'k':
                    channel->setKey(cmd.parameters.empty() ? "" : cmd.parameters.front());
                    if (!cmd.parameters.empty())
                        cmd.parameters.erase(cmd.parameters.begin());
                    break;
                case 'l':
                    channel->setUserLimit(cmd.parameters.empty() ? 0 : std::atoi(cmd.parameters.front().c_str()));
                    if (!cmd.parameters.empty())
                        cmd.parameters.erase(cmd.parameters.begin());
                    break;
                default:
                    server.send_message(client.get_fd(), ERR_UNKNOWNMODE(mode[i]));
                    break;
            }
        }
    }
    else {
        server.send_message(client.get_fd(), ERR_NOSUCHCHANNEL(target));
    }
}
