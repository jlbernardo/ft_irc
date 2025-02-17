#include "ft_irc.hpp"


CommandsManager::CommandsManager(Server &server) : server(server) {
}

CommandsManager::~CommandsManager() {
}

CommandsManager::CommandsManager(const CommandsManager &copy) : server(copy.server) {
    *this = copy;
}

CommandsManager &CommandsManager::operator=(const CommandsManager &copy) {
    if (this != &copy)
        server = copy.server;
    return *this;
}

void CommandsManager::execute(Commands &commands) {
    std::list<Command>& cmd_list = commands.get_list();

    for (std::list<Command>::iterator it = cmd_list.begin(); it != cmd_list.end(); ++it) {
        Command &cmd = *it;
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
            case WHO:
                who(commands, cmd);
                break;
            default:
                break;
        }
    }
    // Clear the list after execution
    commands.clear();
}
