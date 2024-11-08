#pragma once
#include <sstream>
#include <string>

class Message {
 private:
  std::string content;
  int sender_fd;

 public:
  Message(int sender_fd, const std::string& content) : content(content), sender_fd(sender_fd) {}

  std::string format_message() const {
    std::stringstream ss;
    ss << "client" << sender_fd << ": " << content;
    return ss.str();
    // Example output: "client5: Hello everyone!"

    // Later this will return IRC protocol format:
    // ":nickname!username@host PRIVMSG #channel :message content"
  }






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
};
