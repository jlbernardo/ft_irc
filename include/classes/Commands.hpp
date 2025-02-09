#pragma once
#ifndef COMMANDS_HPP
# define COMMANDS_HPP

# include "ft_irc.h"

enum CommandType { PRIVMSG, JOIN, NICK, USER, QUIT, KICK,  PASS, INVITE, TOPIC, MODE, UNKNOWN };

struct Command {
  std::string command;
  std::string params;
  CommandType type;
  std::vector<std::string> parameters;
};


class Commands {

 private:
  std::list<Command> list;
  Client &_sender;
  int _sender_fd;
  bool _fatal_error;
  std::map<std::string, CommandType> _command_map;

  void init_command_map();
  void init_list_of_commands(const std::string &raw_input);
  std::vector<std::string> separate_multiple_commands(const std::string &input, const std::string &delimiter);
  std::string trim(const std::string &str);
  void to_uppercase(std::string &str);
  CommandType find_command_type(const std::string &command);

 public:
  Commands(Client &sender, const std::string &raw_input);
  Client &get_sender() const;
  const std::list<Command> &get_list() const;
  void set_fatal_error(bool fatal_error) { _fatal_error = fatal_error; }
  bool get_fatal_error() const { return _fatal_error; }
  void clear() { list.clear(); }
};

#endif
