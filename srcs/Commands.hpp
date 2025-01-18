#pragma once
#include <map>
#include <string>
#include <vector>
#include <list>

#include "Client.hpp"

enum CommandType { PRIVMSG, JOIN, NICK, USER, QUIT, PASS, CAP, UNKNOWN };

struct Command {
  CommandType type;
  std::string command;
  std::string params;
};

class Commands {
 private:
  std::string _command;
  std::string _target;
  std::string _content;
  Client &_sender;
  const int _sender_fd;
  std::vector<std::string> _parameters;
  std::map<const std::string, CommandType> _command_map;
  CommandType _command_type;

  std::string trim(const std::string &str);
  void init_command_map();
  void parse_parameters(std::istringstream &iss);
  void parse_content_param(std::istringstream &iss, const std::string &first_param);
  void init_list_of_commands(const std::string &raw_message);
  void set_content_from_params();
  std::string add_parameters_to_message(const std::string &msg) const;
  std::string add_content_to_message(const std::string &msg) const;
  bool validate_command_specific() const;
  CommandType find_command_type(const std::string &cmd);
  int count_r_n(const std::string &input, std::string delimiter);
  std::vector<std::string> separate_multiple_commands(const std::string &input, const std::string &delimiter);
  void to_uppercase(std::string &str);

 public:
  Commands(Client &sender, const std::string &raw_message);
  std::string format_command() const;
  CommandType get_command_type() const;
  std::string get_target() const;
  Client &get_sender() const;
  std::vector<std::string> get_parameters() const;
  bool is_valid() const;
  std::string add_timestamp() const;
  std::string get_sender_info() const;
  bool is_channel_message() const;
  bool is_private_message() const;
  std::list<Command> list;
  bool _fatal_error;
};
