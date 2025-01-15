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


// ERR_ PASS command

#define ERR_NEEDMOREPARAMS(nickname) \
	(":461 " + nickname + " :Not enough parameters." + CRLF)

#define ERR_ALREADYREGISTERED(nickname) \
	(":462 " + nickname + " :You may not reregister !" + CRLF)

#define ERR_PASSWDMISMATCH(nickname) \
	(":464 " + nickname + " :Password incorrect !" + CRLF)

/*
 This message is sent from a server to a client to report a fatal error, before terminating the client’s connection.
 This MUST only be used to report fatal errors. Regular errors should use the appropriate numerics or the IRCv3 standard replies framework.
*/
#define ERROR(message) \
	(":ERROR " + message + CRLF)

#endif
