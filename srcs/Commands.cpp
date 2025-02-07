#include <iostream>
#include <sstream>

#include "Commands.hpp"

Commands::Commands(Client &sender, const std::string &raw_input)
    : _sender(sender), _sender_fd(sender.get_fd()), _fatal_error(false) {
  init_command_map();
  init_list_of_commands(raw_input);
}

Client &Commands::get_sender() const { return _sender; }

const std::list<Command> &Commands::get_list() const { return list; }

void Commands::init_command_map() {
  _command_map["PRIVMSG"] = PRIVMSG;
  _command_map["JOIN"] = JOIN;
  _command_map["NICK"] = NICK;
  _command_map["USER"] = USER;
  _command_map["QUIT"] = QUIT;
  _command_map["KICK"] = KICK;
  _command_map["PASS"] = PASS;
  _command_map["INVITE"] = INVITE;
  _command_map["TOPIC"] = TOPIC;
  _command_map["MODE"] = MODE;
}

void Commands::init_list_of_commands(const std::string &raw_input) {
  std::istringstream iss(raw_input);
  std::vector<std::string> vec;
  vec = separate_multiple_commands(raw_input, "\r\n");
  for (std::vector<std::string>::iterator it = vec.begin(); it != vec.end(); it++) {
    std::istringstream cmd(*it);
    Command cmd_obj;
    cmd >> cmd_obj.command;
    getline(cmd, cmd_obj.params);
    cmd_obj.params = trim(cmd_obj.params);
    to_uppercase(cmd_obj.command);
    cmd_obj.type = find_command_type(cmd_obj.command);

    // Parse parameters
    std::istringstream param_iss(cmd_obj.params);
    std::string param;
    while (param_iss >> param) {
      cmd_obj.parameters.push_back(param);
    }

    this->list.push_back(cmd_obj);
  }

  // Debug print to verify commands and parameters
  for (std::list<Command>::const_iterator it = list.begin(); it != list.end(); ++it) {
    std::cout << "Command: " << it->command << ", Parameters: ";
    for (std::vector<std::string>::const_iterator param_it = it->parameters.begin(); param_it != it->parameters.end();
         ++param_it) {
      std::cout << *param_it << " ";
    }
    std::cout << std::endl;
  }
}

std::vector<std::string> Commands::separate_multiple_commands(const std::string &input, const std::string &delimiter) {
  std::vector<std::string> result;
  size_t start = 0;
  while (start < input.length()) {
    size_t end = input.find_first_of(delimiter, start);
    if (end == std::string::npos) {
      result.push_back(input.substr(start));
      break;
    }
    result.push_back(input.substr(start, end - start));
    start = end + delimiter.length();
  }
  return result;
}

std::string Commands::trim(const std::string &str) {
  if (str.empty()) {
    return str;
  }
  // Find first non-whitespace
  std::string::size_type first = str.find_first_not_of(" \t\n\r\f\v");
  if (first == std::string::npos) {
    return "";  // String is all whitespace
  }
  // Find last non-whitespace
  std::string::size_type last = str.find_last_not_of(" \t\n\r\f\v");
  return str.substr(first, (last - first + 1));
}

void Commands::to_uppercase(std::string &str) {
  for (unsigned int i = 0; i < str.length(); i++) {
    str[i] = std::toupper(str[i]);
  }
}

CommandType Commands::find_command_type(const std::string &cmd) {
  if (_command_map.find(cmd) != _command_map.end()) {
    return _command_map[cmd];
  }
  return UNKNOWN;
}
