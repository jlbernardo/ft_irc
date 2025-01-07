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
    : sender(sender), sender_fd(sender.get_fd()) {
  command_map["PRIVMSG"] = PRIVMSG;
  command_map["JOIN"] = JOIN;
  command_map["NICK"] = NICK;
  command_map["USER"] = USER;
  command_map["QUIT"] = QUIT;
  parse_message_components(raw_message);
}

void Parser::parse_message_components(const std::string &input) { // here
  
  std::istringstream iss(input);
  commands_left = split_raw_message(input, CRLF);
  // here we need to check also if the first thing that the string has is really a command, because if we dont, the isstream will be parsed wrong
  // e.g: :user42 NICK user43

  iss >> command >> target;
  to_uppercase(command);
  command_type = parse_command_type(command);
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
  if (command_map.find(cmd) != command_map.end()) {
    return command_map[cmd];
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
    parameters.push_back(param);
  }
}

void Parser::parse_content_param(std::istringstream &iss,
                                 const std::string &first_param) {
  std::string remaining;
  getline(iss, remaining);
  parameters.push_back(first_param.substr(1) + remaining);
  // this is reconnecting the string with the part that was taken off by the iss when the function parse_parameters were doing its thing
  // e.g. ":example rest of the command" would got here as iss = rest of the command, and first_param as :example
  // thus the final string is the concatenation of first_param with the iss without the ":", which is "example rest of the command" 
}

void Parser::set_content_from_params() {
  if (!parameters.empty()) {
    content = parameters.back();
    parameters.pop_back();
  }
}

std::string Parser::add_parameters_to_message(const std::string &msg) const {
  std::string result = msg;
  for (std::vector<std::string>::const_iterator it = parameters.begin();
       it != parameters.end(); ++it) {
    result += " " + *it;
  }
  return result;
}

std::string Parser::add_content_to_message(const std::string &msg) const {
  if (content.empty()) {
    return msg;
  }
  return msg + " :" + content + "\n";
}

std::string Parser::format_message() const {
  std::string formatted =
      ":" + get_sender_info() + " " + command + " " + target;
  formatted = add_parameters_to_message(formatted);
  return add_content_to_message(formatted);
}

std::string Parser::get_sender_info() const {
  return sender.get_nickname() + "!" + sender.get_username() + "@" +
         sender.get_hostname();
}

std::string Parser::add_timestamp() const {
  char timestamp[32];
  time_t now = time(0);
  strftime(timestamp, sizeof(timestamp), "[%H:%M:%S] ", localtime(&now));
  return std::string(timestamp) + format_message();
}

bool Parser::is_valid() const {
  if (command.empty() || target.empty()) {
    return false;
  }
  return validate_command_specific();
}

std::string Parser::get_current_command() {
  return commands_left[0];
}

Client &Parser::get_sender() const { return sender; }

bool Parser::validate_command_specific() const {
  switch (command_type) {
  case PRIVMSG:
    return !content.empty();
  case JOIN:
    return target[0] == '#';
  case NICK:
    return !target.empty();
  case USER:
    return parameters.size() >= 3;
  case QUIT:
    return true;
  default:
    return false;
  }
}
