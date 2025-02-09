#pragma once
#ifndef FT_IRC_H
# define FT_IRC_H

# include <map>
# include <queue>
# include <ctime>
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
# define ORANGE     "\033[0;38;5;166m"
# define GREY       "\033[38;5;244m"
# define DFT        "\033[0m"

Logger log = Logger::getInstance();
Validator validate = Validator();

#endif