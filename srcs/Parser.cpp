#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <ctime>
#include "Client.hpp"
#include "Parser.hpp"

Parser::Parser(const Client &sender, const std::string &raw_message)
    : sender(sender), sender_fd(sender.get_fd()) {
  command_map["PRIVMSG"] = PRIVMSG;
  command_map["JOIN"] = JOIN;
  command_map["NICK"] = NICK;
  command_map["USER"] = USER;
  command_map["QUIT"] = QUIT;
  parse_message_components(raw_message);
}

void Parser::parse_message_components(const std::string &input) {
  std::istringstream iss(input);
  iss >> command >> target;
  command_type = parse_command_type(command);
  parse_parameters(input);
}

CommandType Parser::parse_command_type(const std::string &cmd) {
  if (command_map.find(cmd) != command_map.end()) {
    return command_map[cmd];
  }
  return UNKNOWN;
}

void Parser::parse_parameters(const std::string &msg) {
  std::istringstream iss(msg);
  std::string param;
  iss >> param >> param;  // Skip command and target
  parse_remaining_params(iss);
}

void Parser::parse_remaining_params(std::istringstream &iss) {
  std::string param;
  while (iss >> param) {
    if (param[0] == ':') {
      parse_content_param(iss, param);
      break;
    }
    parameters.push_back(param);
  }
}

void Parser::parse_content_param(std::istringstream &iss, const std::string &first_param) {
  std::string remaining;
  getline(iss, remaining);
  parameters.push_back(first_param.substr(1) + remaining);
}

void Parser::set_content_from_params() {
  if (!parameters.empty()) {
    content = parameters.back();
    parameters.pop_back();
  }
}

std::string Parser::add_parameters_to_message(const std::string &msg) const {
  std::string result = msg;
  for (std::vector<std::string>::const_iterator it = parameters.begin(); it != parameters.end(); ++it) {
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
  std::string formatted = ":" + get_sender_info() + " " + command + " " + target;
  formatted = add_parameters_to_message(formatted);
  return add_content_to_message(formatted);
}

std::string Parser::get_sender_info() const {
  return sender.get_nickname() + "!" + sender.get_username() + "@" + sender.get_hostname();
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

int Parser::get_sender() const {
  return sender_fd;
}

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