#pragma once
#ifndef FT_IRC_HPP
# define FT_IRC_HPP

# include <map>
# include <list>
# include <queue>
# include <ctime>
# include <vector>
# include <string>
# include <sstream>
# include <csignal>
# include <cstring>
# include <cstdlib>
# include <ctype.h>
# include <unistd.h>
# include <iostream>
# include <stdexcept>
# include <arpa/inet.h>
# include <sys/select.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include "replies.hpp"

class Logger;
class Server;
class Client;
class Channel;
class Commands;
class Validator;
class SocketsManager;
class CommandsManager;

# include "classes/Logger.hpp"
# include "classes/Server.hpp"
# include "classes/Client.hpp"
# include "classes/Channel.hpp"
# include "classes/Commands.hpp"
# include "classes/Validator.hpp"
# include "classes/SocketsManager.hpp"
# include "classes/CommandsManager.hpp"

# define EXIT_OK  0
# define EXIT_ERR 1

# define BLUE_GREEN "\033[32;44m"
# define MAG_YE     "\033[33;45m"
# define RED        "\033[1;31m"
# define GREEN      "\033[1;32m"
# define YELLOW     "\033[1;33m"
# define BLUE       "\033[1;34m"
# define MAGENTA    "\033[1;35m"
# define CYAN       "\033[1;36m"
# define WHITE      "\033[1;37m"
# define ORANGE     "\033[1;38;5;166m"
# define GREY       "\033[38;5;244m"
# define DFT        "\033[0m"

static Logger logger = Logger::getInstance();
static Validator validator = Validator();

std::string trim(const std::string &str);
std::string timestamp(const char *format);
std::vector<std::string> split(const std::string &str, char delim);

void privmsg(Commands &commands, Command &cmd);
void join(Commands &commands, Command &cmd);
void nick(Commands &commands, Command &cmd);
void user(Commands &commands, Command &cmd);
void quit(Commands &commands, Command &cmd);
void pass(Commands &commands, Command &cmd);
void mode(Commands &commands, Command &cmd);
void who(Commands &commands, Command &cmd);
void kick(Commands &commands, Command &cmd);
void invite(Commands &commands, Command &cmd);
void topic(Commands &commands, Command &cmd);
void part(Commands &commands, Command &cmd);

#endif