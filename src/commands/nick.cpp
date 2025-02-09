#include "ft_irc.h"


bool is_nickname_in_use(Server &server, const std::string &new_nick) {
    for (std::map<int, Client *>::iterator it = server.get_clients().begin(); it != server.get_clients().end(); ++it) {
        if (it->second->get_nickname() == new_nick)
            return true;
    }
    return false;
}

void nick(Commands &commands, const Command &cmd) {
    Client &sender = commands.get_sender();
    Server &server = sender.getServer();
    const std::string &new_nick = cmd.parameters.empty() ? "" : cmd.parameters[0];
    
    if (new_nick.empty()) {
        server.send_message(sender.get_fd(), ERR_NONICKNAMEGIVEN());
        return;
    }
    else if (is_nickname_in_use(server, new_nick)) {
        server.send_message(sender.get_fd(), ERR_NICKNAMEINUSE(new_nick));
        return;
    }
    std::string old_nick = sender.get_nickname();
    sender.set_nickname(new_nick);
    
    const std::string nick_change_msg = ":" + old_nick + " NICK " + new_nick + "\r\n";

    for (std::map<int, Client *>::iterator it = server.get_clients().begin(); it != server.get_clients().end(); ++it) {
        int client_fd = it->first;

        if (client_fd != sender.get_fd())
            server.get_message_queues()[client_fd].push(nick_change_msg);
    }
}
