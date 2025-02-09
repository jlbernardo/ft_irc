#include "ft_irc.h"


void pass(Commands &commands, const Command &cmd) {
    Client &sender = commands.get_sender();
    Server &server = sender.getServer();
    const std::string &pass = cmd.parameters.empty() ? "" : cmd.parameters[0];

    if (pass.empty()) {
        server.send_message(sender.get_fd(), ERR_NEEDMOREPARAMS(cmd.command));
    }
    else if (!sender.get_pass().empty()) {
        server.send_message(sender.get_fd(), ERR_ALREADYREGISTERED(sender.get_username()));
    }
    else if (pass == server.get_pass()) {
        sender.set_password(pass);

        if (!sender.get_nickname().empty() && !sender.get_username().empty()) {
            sender.set_authentication(true);

            server.send_message(sender.get_fd(), RPL_WELCOME(sender.get_username(), sender.get_identifier()));
            server.send_message(sender.get_fd(), RPL_YOURHOST(sender.get_nickname()));
            server.send_message(sender.get_fd(), RPL_CREATED(sender.get_nickname(), server.get_startup_date()));
            server.send_message(sender.get_fd(), RPL_MYINFO(sender.get_nickname(), "", ""));
        }
    }
    else {
        server.send_message(sender.get_fd(), ERR_PASSWDMISMATCH());
        commands.set_fatal_error(true);
    }
}