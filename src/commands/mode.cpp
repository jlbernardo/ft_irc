#include "ft_irc.hpp"


void mode(Commands &commands, Command &cmd) {
    Client &client = commands.get_sender();
    Server &server = client.getServer();
    std::string target = cmd.parameters.front();
    cmd.parameters.erase(cmd.parameters.begin());

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
        server.send_message(client.get_fd(), ERR_CHANOPRIVSNEEDED(client.get_nickname(), target));
        return ;
    }

    std::string mode = cmd.parameters.front();
    cmd.parameters.erase(cmd.parameters.begin());
    Client *new_op;
    char flag = mode[0];
    std::string argument = "";

    for (std::vector<std::string>::iterator it = cmd.parameters.begin(); it != cmd.parameters.end(); it++)
        argument += " " + *it;

    for (size_t i = 1; i < mode.size(); i++) {
        switch (mode[i]) {
            case '+':
                flag = '+';
                break;
            case '-':
                flag = '-';
                break;
            case 'i':
                channel->setInviteOnly(flag == '+' ? true : false);
                break;
            case 't':
                channel->setTopicRestricted(flag == '+' ? true : false);
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
            case 'o':
                if (cmd.parameters.empty())
                    break;
                
                if (!server.clientExists(cmd.parameters.front())) {
                    server.send_message(client.get_fd(), ERR_NOSUCHNICK(cmd.parameters.front()));
                    break;
                }

                new_op = &server.getClient(cmd.parameters.front());
                cmd.parameters.erase(cmd.parameters.begin());
                
                if (!channel->isMember(new_op)) {
                    server.send_message(client.get_fd(), ERR_USERNOTINCHANNEL(client.get_nickname(), new_op->get_nickname(), target));
                    break;
                }
                if (flag == '+' && !channel->isOperator(new_op))
                    channel->addOperator(new_op);
                if (flag == '-' && channel->isOperator(new_op))
                    channel->removeOperator(new_op);
                break;
            default:
                server.send_message(client.get_fd(), ERR_UNKNOWNMODE(mode[i]));
                break;
        }
    }
    channel->broadcast(&client, RPL_MODE(client.get_identifier(), target, mode, argument));
    
    if (channel->getCurrentOperatorsCount() == 0)
        channel->promoteFirstMember();
}
