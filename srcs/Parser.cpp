#include "Parser.hpp"
#include "Client.hpp"
#include <ctime>
#include <map>
#include <sstream>
#include <string>
#include <vector>
#include "ft_irc.h"
#include <iostream>

Parser::Parser(Client &sender, const std::string &raw_message)
    : _sender(sender), _sender_fd(sender.get_fd()) {
  _command_map["PRIVMSG"] = PRIVMSG;
  _command_map["JOIN"] = JOIN;
  _command_map["NICK"] = NICK;
  _command_map["USER"] = USER;
  _command_map["QUIT"] = QUIT;
  parse_message_components(raw_message);
}

void Parser::parse_message_components(const std::string &input) { // here
  
  std::istringstream iss(input);
  _commands_left = split_raw_message(input, CRLF);
  // here we need to check also if the first thing that the string has is really a command, because if we dont, the isstream will be parsed wrong
  // e.g: :user42 NICK user43

  iss >> _command >> _target;
  to_uppercase(_command);
  _command_type = parse_command_type(_command);
  // the way that the param will be generated, has to acount the command_type structure of each one
  // so the follwoing function should receive the command type as well
  parse_parameters(iss);
}

std::vector<std::string> Parser::split_raw_message(const std::string &input, const std::string &delimiter) {
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

void Parser::to_uppercase(std::string& str) {
    for (unsigned int i = 0; i < str.length(); i++) {
        str[i] = std::toupper(str[i]);
    }
}

int Parser::count_r_n(const std::string &input, std::string delimiter) {
    size_t count = 0;
    size_t pos = 0;

    if (input.length() == 0)
      return 0;
    while ((pos = input.find(delimiter, pos)) != std::string::npos) {
        count++;
        pos += 1;
    }    
    return count;
}

CommandType Parser::parse_command_type(const std::string &cmd) {
  if (_command_map.find(cmd) != _command_map.end()) {
    return _command_map[cmd];
  }
  return UNKNOWN;
}

void Parser::parse_parameters(std::istringstream &iss) {
  std::string param;
  while (iss >> param) {
    if (param[0] == ':') {
      parse_content_param(iss, param);
      break;
    }
    _parameters.push_back(param);
  }
}

void Parser::parse_content_param(std::istringstream &iss,
                                 const std::string &first_param) {
  std::string remaining;
  getline(iss, remaining);
  _parameters.push_back(first_param.substr(1) + remaining);
  // this is reconnecting the string with the part that was taken off by the iss when the function parse_parameters were doing its thing
  // e.g. ":example rest of the command" would got here as iss = rest of the command, and first_param as :example
  // thus the final string is the concatenation of first_param with the iss without the ":", which is "example rest of the command" 
}

void Parser::set_content_from_params() {
  if (!_parameters.empty()) {
    _content = _parameters.back();
    _parameters.pop_back();
  }
}

std::string Parser::add_parameters_to_message(const std::string &msg) const {
  std::string result = msg;
  for (std::vector<std::string>::const_iterator it = _parameters.begin();
       it != _parameters.end(); ++it) {
    result += " " + *it;
  }
  return result;
}

std::string Parser::add_content_to_message(const std::string &msg) const {
  if (_content.empty()) {
    return msg;
  }
  return msg + " :" + _content + "\n";
}

std::string Parser::format_message() const {
  std::string formatted =
      ":" + get_sender_info() + " " + _command + " " + _target;
  formatted = add_parameters_to_message(formatted);
  return add_content_to_message(formatted);
}

std::string Parser::get_sender_info() const {
  return _sender.get_nickname() + "!" + _sender.get_username() + "@" +
         _sender.get_hostname();
}

std::string Parser::add_timestamp() const {
  char timestamp[32];
  time_t now = time(0);
  strftime(timestamp, sizeof(timestamp), "[%H:%M:%S] ", localtime(&now));
  return std::string(timestamp) + format_message();
}

bool Parser::is_valid() const {
  if (_command.empty() || _target.empty()) {
    return false;
  }
  return validate_command_specific();
}

std::string Parser::get_current_command() {
  return _commands_left[0];
}

Client &Parser::get_sender() const { return _sender; }

bool Parser::validate_command_specific() const {
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

CommandType Parser::get_command_type() const { 
  return _command_type; 
}

std::string Parser::get_target() const {
   return _target; 
}

std::vector<std::string> Parser::get_parameters() const {
   return _parameters; 
}

bool Parser::is_channel_message() const {
  return !_target.empty() && _target[0] == '#';
}

bool Parser::is_private_message() const {
  return _command_type == PRIVMSG && !is_channel_message();
}