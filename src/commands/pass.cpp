#include "ft_irc.hpp"


void pass(Commands &commands, Command &cmd) {
    Client &sender = commands.get_sender();
    Server &server = sender.getServer();
    std::string pass = cmd.parameters.empty() ? "" : cmd.parameters.front();

    if (pass.empty()) {
        server.send_message(sender.get_fd(), ERR_NEEDMOREPARAMS(cmd.command));
    }
    else if (!sender.get_pass().empty()) {
        server.send_message(sender.get_fd(), ERR_ALREADYREGISTERED(sender.get_username()));
    }
    else if (pass == server.get_pass()) {
        sender.set_password(pass);

        if (!sender.get_nickname().empty() && !sender.get_username().empty()) {
            std::string nick = sender.get_nickname();
            sender.set_authentication(true);

            sender.set_identifier(nick + "!" + sender.get_username() + "@" + sender.get_hostname());

            server.send_message(sender.get_fd(), RPL_WELCOME(sender.get_username(), sender.get_identifier()));
            server.send_message(sender.get_fd(), RPL_YOURHOST(nick));
            server.send_message(sender.get_fd(), RPL_CREATED(nick, server.get_startup_date()));
            server.send_message(sender.get_fd(), RPL_MYINFO(nick, "", ""));
        }
    }
    else {
        server.send_message(sender.get_fd(), ERR_PASSWDMISMATCH());
        commands.set_fatal_error(true);
    }
}