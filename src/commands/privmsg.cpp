#include "ft_irc.hpp"

void privmsg(Commands &commands, Command &cmd)
{
    Client &sender = commands.get_sender();
    Server &server = sender.getServer();

    if (!sender.is_authenticated())
    {
        server.send_message(sender.get_fd(), ERR_NOTREGISTERED());
        return;
    }

    if (cmd.parameters.empty())
    {
        server.send_message(sender.get_fd(), ERR_NORECIPIENT(sender.get_nickname()));
        return;
    }
    if (cmd.parameters.size() < 2)
    {
        server.send_message(sender.get_fd(), ERR_NOTEXTTOSEND(sender.get_nickname()));
        return;
    }

    std::string &recipient = cmd.parameters[0];
    std::string &message = cmd.parameters[1];

    logger.info("Recipient: " + recipient);
    if (recipient[0] == '#')
    {
        if (server.channelExists(recipient))
        {
            Channel *channel = server.get_channels()[recipient];
            logger.info("Channel found: " + channel->getName());
            if (!channel->isMember(&sender))
            {
                server.send_message(sender.get_fd(), ERR_CANNOTSENDTOCHAN(recipient));
                return;
            }

            for (std::map<int, Client *>::const_iterator it = channel->getMembers().begin(); it != channel->getMembers().end(); ++it)
            {
                Client *member = it->second;

                if (member->get_fd() != sender.get_fd() && !member->is_away())
                    server.send_message(member->get_fd(), RPL_PRIVMSG(sender.get_nickname(), recipient, message));
                else if (member->get_fd() != sender.get_fd() && member->is_away())
                    server.send_message(sender.get_fd(), RPL_AWAY(sender.get_nickname(), member->get_nickname(), member->get_away_message()));
            }
        }
        else
        {
            server.send_message(sender.get_fd(), ERR_NOSUCHCHANNEL(recipient));
        }
    }
    else
    {
        bool userFound = false;
        for (std::map<int, Client *>::iterator it = server.get_clients().begin(); it != server.get_clients().end(); ++it)
        {
            Client *client = it->second;
            if (client->get_nickname() == recipient)
            {
                if (client->is_away())
                {
                    server.send_message(sender.get_fd(), RPL_AWAY(sender.get_nickname(), client->get_nickname(), client->get_away_message()));
                }
                server.send_message(client->get_fd(), RPL_PRIVMSG(sender.get_nickname(), recipient, message));
                userFound = true;
                break;
            }
        }
        if (!userFound)
            server.send_message(sender.get_fd(), ERR_NOSUCHNICK(recipient));
    }
}
