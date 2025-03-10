#include "ft_irc.hpp"

void away(Commands &commands, Command &cmd) {
    Client &sender = commands.get_sender();
	Server &server = sender.getServer();

    if (sender.is_away() && cmd.params.empty()) {
        sender.set_away_status(false);
        sender.set_away_message("");
        server.send_message(sender.get_fd(), RPL_UNAWAY(sender.get_nickname())); 
    }
    else {
        if (!cmd.params.empty()) {}
            sender.set_away_message(cmd.params);
        sender.set_away_status(true);
        server.send_message(sender.get_fd(), RPL_NOWAWAY(sender.get_nickname())); 
    }
    
}