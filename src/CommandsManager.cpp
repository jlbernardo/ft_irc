#include "ft_irc.hpp"


CommandsManager::CommandsManager(Server &server) : server(server) {
}

void CommandsManager::execute(Commands &commands) {
    const std::list<Command>& cmd_list = commands.get_list();

    for (std::list<Command>::const_iterator it = cmd_list.begin(); it != cmd_list.end(); ++it) {
        const Command &cmd = *it;
        switch (cmd.type) {
            case PRIVMSG:
                privmsg(commands, cmd);
                break;
            case JOIN:
                join(commands, cmd);
                break;
            case NICK:
                nick(commands, cmd);
                break;
            case USER:
                user(commands, cmd);
                break;
            case QUIT:
                // quit(commands, cmd);
                break;
            case KICK:
                // kick(commands, cmd);
                break ;
            case PASS:
                pass(commands, cmd);
                break;
            case INVITE:
                // invite();
                break;
            case TOPIC:
                // topic();
                break;
            case MODE:
                mode(commands, cmd);
                break;
            default:
                break;
        }
    }
    // Clear the list after execution
    commands.clear();
}
