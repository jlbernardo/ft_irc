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
        if (commands.get_sender().is_authenticated() == false) {
            if (cmd.type != PASS && cmd.type != NICK && cmd.type != USER && cmd.type != UNKNOWN && cmd.type != CAP) {
                server.send_message(commands.get_sender().get_fd(), ERR_NOTREGISTERED());
                // print entire commands list (the actual strings)
                for (std::list<Command>::iterator it = cmd_list.begin(); it != cmd_list.end(); ++it) {
                    std::cout << "cmd type: " << it->type << std::endl;
                    std::cout << "cmd: " << it->command << std::endl;
                }
                continue;
            }
        }
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
                quit(commands, cmd);
                break;
            case KICK:
                kick(commands, cmd);
                break ;
            case PASS:
                pass(commands, cmd);
                break;
            case INVITE:
                invite(commands, cmd);
                break;
            case TOPIC:
                topic(commands, cmd);
                break;
            case MODE:
                mode(commands, cmd);
                break;
            case WHO:
                who(commands, cmd);
                break;
            case PART:
                part(commands, cmd);
                break;
            case NOTICE:
                notice(commands, cmd);
                break;
            case AWAY:
                away(commands, cmd);
                break;
            case UNKNOWN:
                server.send_message(commands.get_sender().get_fd(), ERR_UNKNOWNCMD(cmd.command));
                break;
            default:
                break;
        }
    }
    // Clear the list after execution
    commands.clear();
}
