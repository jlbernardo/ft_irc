#include <ctime>
#include <sstream>

#include "Message.hpp"

Message::Message(const Client &sender, const std::string &raw_message) : sender(sender), sender_fd(sender.get_fd()) {
  command_map["PRIVMSG"] = PRIVMSG;
  command_map["JOIN"] = JOIN;
  command_map["NICK"] = NICK;
  command_map["USER"] = USER;
  command_map["QUIT"] = QUIT;
  parse_message_components(raw_message);
  parse_parameters(raw_message);
  set_content_from_params();
}

void Message::parse_message_components(const std::string &raw_message) {
  std::istringstream iss(raw_message);
  iss >> command >> target;
  command_type = command_map[command];
}

void Message::parse_parameters(const std::string &msg) {
  std::istringstream iss(msg);
  std::string param;
  iss >> param >> param;  // Skip command and target
  parse_remaining_params(iss);
}

void Message::parse_remaining_params(std::istringstream &iss) {
  std::string param;
  while (iss >> param) {
    if (param[0] == ':') {
      parse_content_param(iss, param);
      break;
    }
    parameters.push_back(param);
  }
}

void Message::parse_content_param(std::istringstream &iss, const std::string &first_param) {
  std::string remaining;
  getline(iss, remaining);
  parameters.push_back(first_param.substr(1) + remaining);
}

void Message::set_content_from_params() {
  if (!parameters.empty()) {
    content = parameters.back();
    parameters.pop_back();
  }
}

std::string Message::format_message() const {
  std::string formatted = ":" + get_sender_info() + " " + command + " " + target;
  formatted = add_parameters_to_message(formatted);
  return add_content_to_message(formatted);
}

std::string Message::add_parameters_to_message(const std::string &msg) const {
  std::string result = msg;
  for (std::vector<std::string>::const_iterator it = parameters.begin(); it != parameters.end(); ++it) {
    result += " " + *it;
  }
  return result;
}

std::string Message::add_content_to_message(const std::string &msg) const {
  if (content.empty()) {
    return msg;
  }
  return msg + " :" + content + "\n";
}

bool Message::is_valid() const {
  if (command.empty() || target.empty()) {
    return false;
  }
  return validate_command_specific();
}

bool Message::validate_command_specific() const {
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

std::string Message::add_timestamp() const {
  char timestamp[32];
  time_t now = time(0);
  strftime(timestamp, sizeof(timestamp), "[%H:%M:%S] ", localtime(&now));
  return std::string(timestamp) + format_message();
}

std::string Message::get_sender_info() const {
  return sender.get_nickname() + "!" + sender.get_username() + "@" + sender.get_hostname();
}
