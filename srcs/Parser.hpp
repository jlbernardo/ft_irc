#pragma once
#include <map>
#include <string>
#include <vector>

#include "Client.hpp"

enum CommandType { PRIVMSG, JOIN, NICK, USER, QUIT, UNKNOWN };

class Parser {
private:
  std::string command;
  std::string target;
  std::string content;
  Client &sender;
  const int sender_fd;
  std::vector<std::string> parameters;
  std::map<const std::string, CommandType> command_map;
  CommandType command_type;

  void parse_parameters(std::istringstream &iss);
  void parse_content_param(std::istringstream &iss,
                           const std::string &first_param);
  void parse_message_components(const std::string &raw_message);
  void set_content_from_params();
  std::string add_parameters_to_message(const std::string &msg) const;
  std::string add_content_to_message(const std::string &msg) const;
  bool validate_command_specific() const;
  CommandType parse_command_type(const std::string &cmd);

public:
  Parser(Client &sender, const std::string &raw_message);
  std::string format_message() const;
  CommandType get_command_type() const { return command_type; }
  std::string get_target() const { return target; }
  Client &get_sender() const;
  std::vector<std::string> get_parameters() const { return parameters; }
  bool is_valid() const;
  std::string add_timestamp() const;
  std::string get_sender_info() const;
  bool is_channel_message() const {
    return !target.empty() && target[0] == '#';
  }
  bool is_private_message() const {
    return command_type == PRIVMSG && !is_channel_message();
  }
};
