#pragma once

#include <iostream>

#include <string>

#define println(x) std::cout << x << std::endl
#define printlnnl(x) std::cout << x
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

const std::string SERVER = ":big.little.talk.irc";
const std::string CRLF = "\r\n";

inline std::string RPL_WELCOME(const std::string& user, const std::string& client_id) {
  return SERVER + " 001 " + user + " :Welcome to the Internet Relay Chat, " + client_id + CRLF;
}

inline std::string RPL_ENDOFWHO(const std::string& channel) {
  return SERVER + " 315 " + channel + " :End of /WHO list." + CRLF;
}

inline std::string RPL_CHANNELMODEIS(const std::string& channel, const std::string& mode,
                                     const std::string& modeParams) {
  return SERVER + " 324 * " + channel + " " + mode + " " + modeParams + CRLF;
}

inline std::string RPL_NOTOPIC(const std::string& nick, const std::string& channel) {
  return SERVER + " 331 " + nick + " " + channel + " :No topic is set" + CRLF;
}

inline std::string RPL_TOPIC(const std::string& nick, const std::string& channel, const std::string& topic) {
  return SERVER + " 332 " + nick + " " + channel + " :" + topic + CRLF;
}

inline std::string RPL_INVITING(const std::string& nick, const std::string& recipient, const std::string& channel) {
  return SERVER + " 341 " + nick + " " + recipient + " :" + channel + CRLF;
}

inline std::string RPL_WHOREPLY(const std::string& channel, const std::string& user, const std::string& nick,
                                const std::string& flags, const std::string& realname) {
  return SERVER + " 352 " + channel + " " + user + " 42sp.org.br ft.irc " + nick + " " + flags + ":0 " + realname +
         CRLF;
}

inline std::string RPL_NAMREPLY(const std::string& nick, const std::string& channel, const std::string& names) {
  return SERVER + " 353 " + nick + " = " + channel + " : " + names + CRLF;
}

inline std::string RPL_ENDOFNAMES(const std::string& nick, const std::string& channel) {
  return SERVER + " 366 " + nick + " " + channel + " : End of names list" + CRLF;
}

// 400s
inline std::string ERR_NOSUCHCHANNEL(const std::string& channel) {
  return SERVER + " 403 * " + channel + " :Invalid channel name!" + CRLF;
}

inline std::string ERR_NOSUCHNICK(const std::string& recipient) {
  return SERVER + " 406 " + recipient + " :No such nick" + CRLF;
}

inline std::string ERR_NORECIPIENT(const std::string& user) {
  return SERVER + " 411 " + user + " :No recipient to message" + CRLF;
}

inline std::string ERR_NOTEXTTOSEND(const std::string& user) {
  return SERVER + " 412 " + user + " :No message to send" + CRLF;
}

inline std::string ERR_NONICKNAMEGIVEN() { return SERVER + " 431 :No nickname given" + CRLF; }

inline std::string ERR_ERRONEUSNICKNAME(const std::string& nick) {
  return SERVER + " 432 * " + nick + " :Nickname is invalid" + CRLF;
}

inline std::string ERR_NICKNAMEINUSE(const std::string& nick) {
  return SERVER + " 433 * " + nick + " :Nickname is already in use" + CRLF;
}

inline std::string ERR_USERNOTINCHANNEL(const std::string& operator_, const std::string& client,
                                        const std::string& channel) {
  return SERVER + " 441 " + operator_ + " " + client + " " + channel + " :They aren't on that channel" + CRLF;
}

inline std::string ERR_NOTONCHANNEL(const std::string& channel) {
  return SERVER + " 442 * " + channel + " :Client not on channel!" + CRLF;
}

inline std::string ERR_USERONCHANNEL(const std::string& nick, const std::string& channel) {
  return SERVER + " 443 * " + nick + " " + channel + " :Client on channel!" + CRLF;
}

inline std::string ERR_NOTREGISTERED() {
  return SERVER + " 451 * :You have not registered" + CRLF;
}

inline std::string ERR_NEEDMOREPARAMS(const std::string& command) {
  return SERVER + " 461 * " + command + " " + ":Not enough parameters" + CRLF;
}

inline std::string ERR_ALREADYREGISTERED(const std::string& user) {
  return SERVER + " 462 " + user + " :User already registered" + CRLF;
}

inline std::string ERR_PASSWDMISMATCH() {
  return SERVER + " 464 * :Password was either not given or was incorrect" + CRLF;
}

inline std::string ERR_CHANNELISFULL(const std::string& channel) {
  return SERVER + " 471 * " + channel + " :Channel is full!" + CRLF;
}

inline std::string ERR_INVITEONLYCHAN(const std::string& channel) {
  return SERVER + " 473 * " + channel + " :Channel is invite-only!" + CRLF;
}

inline std::string ERR_BADCHANNELKEY(const std::string& user, const std::string& channel) {
  return SERVER + " 475 " + user + " " + channel + " :Password for channel was either not given or incorrect" + CRLF;
}

inline std::string ERR_BADCHANMASK(const std::string& channel) {
  return SERVER + "476" + channel + " :Bad Channel Mask" + CRLF;
}

inline std::string ERR_CHANOPRIVSNEEDED(const std::string& user, const std::string& channel) {
  return SERVER + " 482 " + user + " " + channel + " :You're not a channel operator!" + CRLF;
}

inline std::string ERROR(const std::string message) { return ":ERROR " + message + CRLF; }

inline std::string RPL_JOIN(const std::string& client_id, const std::string& channel) {
  return client_id + " JOIN " + channel + CRLF;
}

inline std::string PRIVMSG_BROADCAST(const std::string& nick, const std::string& user, const std::string& channel,
                                     const std::string& topic) {
  return SERVER + nick + "!~" + user + "@ft.irc TOPIC " + channel + " " + topic + CRLF;
}

inline std::string RPL_PRIVMSG(const std::string& client_id, const std::string& dest, const std::string& message) {
  return ":" + client_id + " PRIVMSG " + dest + " :" + message + CRLF;
}

inline std::string RPL_PARTMSG(const std::string& nick, const std::string& user, const std::string& dest,
                               const std::string& message) {
  return SERVER + nick + "!~" + user + "@* PART " + dest + " :" + message + CRLF;
}

inline std::string RPL_PARTNOMSG(const std::string& nick, const std::string& user, const std::string& dest) {
  return SERVER + nick + "!" + user + "@* PART " + dest + CRLF;
}

inline std::string RPL_KICKREASON(const std::string& op_nick, const std::string& op_user, const std::string& channel,
                                  const std::string& client, const std::string& reason) {
  return SERVER + op_nick + "!" + op_user + "@ft.irc KICK " + channel + " " + client + " :" + reason + CRLF;
}

inline std::string RPL_KICKNOREASON(const std::string& op_nick, const std::string& op_user, const std::string& channel,
                                    const std::string& client) {
  return SERVER + op_nick + "!" + op_user + "@ft.irc KICK " + channel + " " + client + CRLF;
}

inline std::string RPL_KICKFEEDBACK(const std::string& op_nick, const std::string& op_user, const std::string& channel,
                                    const std::string& client) {
  return SERVER + op_nick + "!" + op_user + "@ft.irc KICK " + channel + " " + client + CRLF;
}

inline std::string RPL_MODEBASE(const std::string& nick, const std::string& user, const std::string& channel) {
  return SERVER + nick + "!" + user + "@ft.irc MODE " + channel + " ";
}

inline std::string RPL_INVITEMSG(const std::string& nick, const std::string& user, const std::string& recipient,
                                 const std::string& channel) {
  return SERVER + nick + "!~" + user + "@ft.irc INVITE " + recipient + " :" + channel + CRLF;
}

inline std::string RPL_YOURHOST(const std::string& user) {
  return SERVER + " 002 " + user + " :Your host is " + SERVER + ", running version 1.0" + CRLF;
}

#include <ctime>

inline std::string getCurrentDate() {
  std::time_t now = std::time(NULL);
  char buf[100];
  std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
  return std::string(buf);
}

inline std::string RPL_CREATED(const std::string& user) { //change it so it reflects the server's creation date
  return SERVER + " 003 " + user + " :This server was created " + getCurrentDate() + CRLF;
}

inline std::string RPL_MYINFO(const std::string& user, const std::string& userModes, const std::string& channelModes) {
  return SERVER + " 004 " + user + " " + SERVER + " " + "1.0 " + userModes + " " + channelModes + CRLF;
}

inline std::string ERR_NICKCOLLISION(const std::string& nick, const std::string& user, const std::string& host) {
  return SERVER + " 436 " + SERVER + " " + nick + " :Nickname collision KILL from " + user + "@" + host + CRLF;
}
