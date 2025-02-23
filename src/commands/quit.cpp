#include "ft_irc.hpp"


void quit(Commands &commands, Command &cmd) {
    Client &sender = commands.get_sender();
    Server &server = sender.getServer();

    std::string reason = cmd.parameters.size() > 0 ? cmd.parameters[0] : "";
    std::vector<Channel*> channels = sender.get_joined_channels();

    for (std::vector<Channel*>::iterator it = channels.begin(); it != channels.end(); it++) {
        Channel *channel = *it;
        std::string message = RPL_PARTMSG(sender.get_identifier(), channel->getName(), reason);

        channel->broadcast(&sender, message);
        channel->removeMember(&sender);
        sender.remove_channel(channel);

        if (channel->isOperator(&sender))
            channel->removeOperator(&sender);

        if (channel->getCurrentOperatorsCount() == 0)
            channel->promoteFirstMember();

        if (channel->getCurrentMembersCount() == 0) {
            server.get_channels().erase(channel->getName());
            delete channel;
        }
    }
    server.send_message(sender.get_fd(),ERROR("Quit: " + reason));
}
