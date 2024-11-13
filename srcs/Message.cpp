#include <sstream>

#include "Message.hpp"

Message::Message(const Client &sender)
    : command("PRIVMSG"), target("#main"), content(sender.get_buffer()), sender(sender), sender_fd(sender.get_fd()) {};

std::string Message::format_message() const {
  std::stringstream ss;
  ss << ":"
	 << sender.get_nickname()
	 << "!" << sender.get_username()
	 << "@" << sender.get_hostname()
	 << " " << command
     << " " << target << " :" << content;
  return ss.str();
}
