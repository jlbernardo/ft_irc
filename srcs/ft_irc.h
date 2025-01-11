#ifndef FT_IRC_H
#define FT_IRC_H

#define println(x) std::cout << x << DFT << std::endl;
#define printlnnl(x) std::cout << x << DFT << "\n" << std::endl;
#define errorln(x) std::cerr << RED << x << DFT << std::endl;

#define BLUE_GREEN	"\033[32;44m"
#define MAG_YE		"\033[33;45m"
#define RED     	"\033[1;31m"
#define GREEN   	"\033[1;32m"
#define YELLOW 		"\033[1;33m"
#define BLUE    	"\033[1;34m"
#define MAGENTA    	"\033[1;35m"
#define CYAN    	"\033[1;36m"
#define WHITE    	"\033[1;37m"
#define ORANGE		"\033[0;38;5;166m"
#define GREY		"\033[38;5;244m"
#define DFT   		"\033[0m"
#define CRLF        "\r\n"

// RPL_

#define RPL_CONNECTED(serverhostname, nickname, targethostname) \
	(":" + serverhostname + " 001 " + nickname +                \
	 " :Welcome to the IRC server! " + nickname + "!" + targethostname + CRLF)

#define RPL_YOURHOST(serverhostname, nickname, targethostname) \
	(":" + serverhostname + " 002 " + nickname + " :Welcome to the IRC server! " + nickname + "!" + targethostname + CRLF)
    // <client> :Your host is <servername>, running version <version>

#define RPL_CREATED(serverhostname, nickname, targethostname) \
	(":" + serverhostname + " 003 " + nickname + " :Welcome to the IRC server! " + nickname + "!" + targethostname + CRLF)
    // <client> :This server was created <datetime>

#define RPL_MYINFO(serverhostname, nickname, targethostname) \
	(":" + serverhostname + " 004 " + nickname + " :Welcome to the IRC server! " + nickname + "!" + targethostname + CRLF)
    // <client> <servername> <version> <available user modes> <available channel modes> [<channel modes with a parameter>]

//ERR_ NICK()

// #define ERR_NONICKNAMEGIVEN(serverhostname, nickname, targethostname) \ 431
// #define ERR_ERRONEUSNICKNAME(serverhostname, nickname, targethostname) \ 432
// #define ERR_NICKNAMEINUSE(serverhostname, nickname, targethostname) \ 433
// #define ERR_NICKCOLLISION(serverhostname, nickname, targethostname) \ 436

//ERR_ USER()
// ERR_NEEDMOREPARAMS (461)
// ERR_ALREADYREGISTERED (462)

#endif