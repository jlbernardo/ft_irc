#pragma once
#include <string>
#include <vector>
#include <map>

#include "Client.hpp"

enum CommandType { PRIVMSG, JOIN, NICK, USER, QUIT, UNKNOWN };

class Parser {
 private:
  std::string command;
  std::string target;
  std::string content;
  const Client &sender;
  const int sender_fd;
  std::vector<std::string> parameters;
  std::map<const std::string, CommandType> command_map;
  void parse_remaining_params(std::istringstream &iss);
  void parse_content_param(std::istringstream &iss, const std::string &first_param);
  void parse_message_components(const std::string &raw_message);
  void set_content_from_params();
  std::string add_parameters_to_message(const std::string &msg) const;
  std::string add_content_to_message(const std::string &msg) const;
  bool validate_command_specific() const;

  CommandType command_type;
  void parse_parameters(const std::string &msg);
  CommandType parse_command_type(const std::string &cmd);

 public:
  Parser(const Client &sender, const std::string &raw_message);

  // Formats message as ":nickname!username@host COMMAND target params :content"
  std::string format_message() const;

  CommandType get_command_type() const { return command_type; }
  std::string get_target() const { return target; }
  std::vector<std::string> get_parameters() const { return parameters; }

  bool is_valid() const;
  std::string add_timestamp() const;
  std::string get_sender_info() const;
  bool is_channel_message() const { return !target.empty() && target[0] == '#'; }
  bool is_private_message() const { return command_type == PRIVMSG && !is_channel_message(); }
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
