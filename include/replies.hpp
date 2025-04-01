#pragma once
#ifndef REPLIES_HPP
# define REPLIES_HPP

# include <string>

const std::string SERVER = ":big.little.talk.irc";
const std::string CRLF = "\r\n";

inline std::string RPL_WELCOME(const std::string& nick, const std::string& client_id) {
  return SERVER + " 001 " + nick + " :Welcome to the Internet Relay Chat, " + client_id + CRLF;
}

inline std::string RPL_YOURHOST(const std::string& nick) {
  return SERVER + " 002 " + nick + " :Your host is " + SERVER + ", running version 1.0" + CRLF;
}

inline std::string RPL_CREATED(const std::string& nick, std::string& startup) {
  return SERVER + " 003 " + nick + " :This server was created on " + startup + CRLF;
}

inline std::string RPL_MYINFO(const std::string& nick, const std::string& userModes, const std::string& channelModes) {
  return SERVER + " 004 " + nick + " " + SERVER + " " + "1.0 " + userModes + " " + channelModes + CRLF;
}

inline std::string RPL_AWAY(const std::string& nick, const std::string& recipient, const std::string& away_message) {
  return SERVER + " 301 " + nick + " " + recipient + " :" + away_message + CRLF;
}

inline std::string RPL_UNAWAY(const std::string& nick) {
  return SERVER + " 305 " + nick + " :You are no longer marked as being away." + CRLF;
}

inline std::string RPL_NOWAWAY(const std::string& nick) {
  return SERVER + " 306 " + nick + " :You have been marked as being away." + CRLF;
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
  return SERVER + " 341 " + nick + " " + recipient + " " + channel + CRLF;
}

inline std::string RPL_WHOREPLY(const std::string& channel, const std::string& user, const std::string &host,
                                const std::string& nick, const std::string& flags, const std::string& realname) {
  return SERVER + " 352 " + channel + " " + user + " " + host + " big.little.talk.irc " + nick + " " + flags +
         " :0 " + realname + CRLF;
}

inline std::string RPL_NAMREPLY(const std::string& nick, const std::string& channel, const std::string& names) {
  return SERVER + " 353 " + nick + " = " + channel + " :" + names + CRLF;
}

inline std::string RPL_ENDOFNAMES(const std::string& nick, const std::string& channel) {
  return SERVER + " 366 " + nick + " " + channel + " :End of names list" + CRLF;
}

// 400s
inline std::string ERR_NOSUCHCHANNEL(const std::string& channel) {
  return SERVER + " 403 " + channel + " :Invalid channel name!" + CRLF;
}

inline std::string ERR_CANNOTSENDTOCHAN(const std::string& channel) {
  return SERVER + " 404 * " + channel + " :Cannot send to channel" + CRLF;
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

inline std::string ERR_NONICKNAMEGIVEN() {
  return SERVER + " 431 :No nickname given" + CRLF;
}

inline std::string ERR_ERRONEUSNICKNAME(const std::string& nick) {
  return SERVER + " 432 * " + nick + " :Nickname is invalid" + CRLF;
}

inline std::string ERR_NICKNAMEINUSE(const std::string& nick) {
  return SERVER + " 433 * " + nick + " :Nickname is already in use" + CRLF;
}

inline std::string ERR_NICKCOLLISION(const std::string& nick, const std::string& user, const std::string& host) {
  return SERVER + " 436 " + SERVER + " " + nick + " :Nickname collision KILL from " + user + "@" + host + CRLF;
}

inline std::string ERR_USERNOTINCHANNEL(const std::string& oper, const std::string& client,
                                        const std::string& channel) {
  return SERVER + " 441 " + oper + " " + client + " " + channel + " :They aren't on that channel!" + CRLF;
}

inline std::string ERR_NOTONCHANNEL(const std::string& channel) {
  return SERVER + " 442 * " + channel + " :You're not on that channel!" + CRLF;
}

inline std::string ERR_USERONCHANNEL(const std::string& nick, const std::string& channel) {
  return SERVER + " 443 * " + nick + " " + channel + " :is already on channel!" + CRLF;
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
  return SERVER + " 464 * :pass was either not given or was incorrect" + CRLF;
}

inline std::string ERR_CHANNELISFULL(const std::string& channel) {
  return SERVER + " 471 * " + channel + " :Channel is full!" + CRLF;
}

inline std::string ERR_UNKNOWNMODE(char mode) {
  return SERVER + " 472 * " + mode + " :is unknown mode char to me" + CRLF;
}

inline std::string ERR_INVITEONLYCHAN(const std::string& channel) {
  return SERVER + " 473 * " + channel + " :Channel is invite-only!" + CRLF;
}

inline std::string ERR_BADCHANNELKEY(const std::string& user, const std::string& channel) {
  return SERVER + " 475 " + user + " " + channel + " :pass for channel was either not given or incorrect" + CRLF;
}

inline std::string ERR_BADCHANMASK(const std::string& channel) {
  return SERVER + " 476 " + channel + " :Bad Channel Mask" + CRLF;
}

inline std::string ERR_CHANOPRIVSNEEDED(const std::string& user, const std::string& channel) {
  return SERVER + " 482 " + user + " " + channel + " :You're not a channel operator!" + CRLF;
}

inline std::string ERROR(const std::string message) {
  return ":ERROR " + message + CRLF;
}

inline std::string RPL_JOIN(const std::string& user, const std::string& channel) {
  return ":" + user + " JOIN " + channel + CRLF;
}

inline std::string RPL_PRIVMSG(const std::string& client_id, const std::string& dest, const std::string& message) {
  return ":" + client_id + " PRIVMSG " + dest + " :" + message + CRLF;
}

inline std::string RPL_NOTICE(const std::string& client_id, const std::string& dest, const std::string& message) {
  return ":" + client_id + " NOTICE " + dest + " :" + message + CRLF;
}

inline std::string RPL_PARTMSG(const std::string& identifier, const std::string& dest, const std::string& message) {
  return ":" + identifier + " PART " + dest + " :" + message + CRLF;
}

inline std::string RPL_KICK(const std::string& identifier, const std::string& channel, const std::string& target, const std::string& reason) {
  return ":" + identifier + " KICK " + channel + " " + target + " :" + reason + CRLF;
}

inline std::string RPL_TOPICCHANGE(const std::string sender, const std::string& channel, const std::string& topic) {
  return ":" + sender + " TOPIC " + channel + " :" + topic + CRLF;
}

inline std::string RPL_MODE(const std::string sender, const std::string& channel, const std::string& mode, const std::string& target) {
  return ":" + sender + " MODE " + channel + " " + mode + " " + target + CRLF;
}

inline std::string RPL_INVITE(const std::string& nick, const std::string& recipient, const std::string& channel) {
  return ":" + nick + " INVITE " + recipient + " " + channel + CRLF;
}

inline std::string ERR_UNKNOWNCMD(const std::string& command) {
  return SERVER + " 421 * " + command + " :Unknown command" + CRLF;
}

#endif
