#include "ft_irc.h"


void pass(Commands &commands, const Command &cmd) {
    Client &client = commands.get_sender();
    Server &server = client.getServer();
    const std::string &pass = cmd.parameters.empty() ? "" : cmd.parameters[0];

    if (pass.empty()) {
        server.send_message(client.get_fd(), ERR_NEEDMOREPARAMS(cmd.command));
    }
    else if (!client.get_pass().empty()) {
        server.send_message(client.get_fd(), ERR_ALREADYREGISTERED(client.get_username()));
    }
    else if (pass == server.get_pass()) {
        client.set_password(pass);
        
        if (!client.get_nickname().empty() && !client.get_username().empty()) {
            client.set_authentication(true);
            server.send_message(client.get_fd(), RPL_WELCOME(client.get_username(), client.get_identifier()));
        }
    }
    else {
        server.send_message(client.get_fd(), ERR_PASSWDMISMATCH());
        commands.set_fatal_error(true);
    }
}