#include <ctime>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "Client.hpp"
#include "Commands.hpp"
#include "ft_irc.h"

Commands::Commands(Client &sender, const std::string &raw_input) : _sender(sender), _sender_fd(sender.get_fd()), _fatal_error(false) {
  init_command_map();
  init_list_of_commands(raw_input);
}

void Commands::init_command_map() {
  _command_map["PRIVMSG"] = PRIVMSG;
  _command_map["JOIN"] = JOIN;
  _command_map["NICK"] = NICK;
  _command_map["USER"] = USER;
  _command_map["QUIT"] = QUIT;
  _command_map["PASS"] = PASS;
}

void Commands::init_list_of_commands(const std::string &raw_input) {
  std::istringstream iss(raw_input);
  std::vector<std::string> vec;
  vec = separate_multiple_commands(raw_input, CRLF);
  for (std::vector<std::string>::iterator it = vec.begin(); it != vec.end(); it++) {
    std::istringstream cmd(*it);
    Command cmd_obj;
    cmd >> cmd_obj.command;
    getline(cmd, cmd_obj.params);
    cmd_obj.params = trim(cmd_obj.params);
    to_uppercase(cmd_obj.command);
    cmd_obj.type = find_command_type(cmd_obj.command);
    this->list.push_back(cmd_obj);
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

    start = end + 2;
  }
  return result;
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

std::string Commands::format_command() const {
  std::string formatted = ":" + get_sender_info() + " " + _command + " " + _target;
  formatted = add_parameters_to_message(formatted);
  return add_content_to_message(formatted);
}

std::string Commands::get_sender_info() const {
  return _sender.get_nickname() + "!" + _sender.get_username() + "@" + _sender.get_hostname();
}

std::string Commands::add_parameters_to_message(const std::string &msg) const {
  std::string result = msg;
  for (std::vector<std::string>::const_iterator it = _parameters.begin(); it != _parameters.end(); ++it) {
    result += " " + *it;
  }
  return result;
}

std::string Commands::add_content_to_message(const std::string &msg) const {
  if (_content.empty()) {
    return msg;
  }
  return msg + " :" + _content + "\n";
}

std::string Commands::add_timestamp() const {
  char timestamp[32];
  time_t now = time(0);
  strftime(timestamp, sizeof(timestamp), "[%H:%M:%S] ", localtime(&now));
  return std::string(timestamp) + format_command();
}

bool Commands::is_valid() const {
  if (_command.empty() || _target.empty()) {
    return false;
  }
  return validate_command_specific();
}

Client &Commands::get_sender() const { return _sender; }

bool Commands::validate_command_specific() const {
  switch (_command_type) {
    case PRIVMSG:
      return !_content.empty();
    case JOIN:
      return _target[0] == '#';
    case NICK:
      return !_target.empty();
    case USER:
      return _parameters.size() >= 3;
    case QUIT:
      return true;
    default:
      return false;
  }
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

std::vector<std::string> Commands::get_parameters() const { return _parameters; }

// COMMENTED OUT BECAUSE WE'RE NOT USING IT AND I DON'T UNDERSTAND IT'S PURPOSE
// void Command::parse_parameters(std::istringstream &iss) {
//   std::string param;
//   while (iss >> param) {
//     if (param[0] == ':') {
//       parse_content_param(iss, param);
//       break;
//     }
//     _parameters.push_back(param);
//   }
// }
//
// void Command::parse_content_param(std::istringstream &iss, const std::string &first_param) {
//   std::string remaining;
//   getline(iss, remaining);
//   _parameters.push_back(first_param.substr(1) + remaining);
//   // this is reconnecting the string with the part that was taken off by the iss when the function parse_parameters
//   were
//   // doing its thing e.g. ":example rest of the command" would got here as iss = rest of the command, and first_param
//   as
//   // :example thus the final string is the concatenation of first_param with the iss without the ":", which is
//   "example
//   // rest of the command"
// }

// void Command::set_content_from_params() {
//   if (!_parameters.empty()) {
//     _content = _parameters.back();
//     _parameters.pop_back();
//   }
// }

// int Tokenizer::count_r_n(const std::string &input, std::string delimiter) {
//   size_t count = 0;
//   size_t pos = 0;
//   if (input.length() == 0) return 0;
//   while ((pos = input.find(delimiter, pos)) != std::string::npos) {
//     count++;
//     pos += 1;
//   }
//   return count;
// }

// CommandType Command::get_command_type() const { return _command_type; }
//
// std::string Command::get_target() const { return _target; }
//
//
// bool Command::is_channel_message() const { return !_target.empty() && _target[0] == '#'; }
//
// bool Command::is_private_message() const { return _command_type == PRIVMSG && !is_channel_message(); }
//
