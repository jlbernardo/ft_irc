#pragma once
#include "Client.hpp"
#include <string>

class Message {
 private:
  const std::string command;
  const std::string target;
  const std::string content;
  const Client &sender;
  const int sender_fd;

 public:
  Message(const Client &sender);
  std::string format_message() const;
    // ":nickname!username@host PRIVMSG #channel :message content"
};






  // Later we might have to add:
  // - parse_command()      -> Will extract IRC commands (PRIVMSG, JOIN, NICK, etc.)
  // - get_command_type()   -> Returns enum of command type
  // - get_target()         -> Gets target user/channel
  // - get_parameters()     -> Gets command parameters
  // - is_valid()          -> Checks if message follows IRC protocol
  // - format_irc()        -> Formats as IRC protocol message (:prefix COMMAND params :trailing)
  // - add_timestamp()     -> Adds server timestamp
  // - add_prefix()        -> Adds sender info prefix
  // - is_channel_message()-> Checks if message is for channel
  // - is_private_message()-> Checks if message is private
  // - get_sender_info()   -> Returns sender nickname/username
