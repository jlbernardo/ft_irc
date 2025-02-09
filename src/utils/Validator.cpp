#include "ft_irc.h"

Validator::Validator() {
}

Validator::~Validator() {
}

void Validator::args(int argc) {
  if (argc != 3) {
	throw std::invalid_argument("Invalid number of arguments");
  }
}

void Validator::port(const char* port_str) {
  for (const char* c = port_str; *c != '\0'; ++c) {
	if (!std::isdigit(*c))
	  throw std::invalid_argument("Port must be a number");
  }

  int port = std::atoi(port_str);
  if (port <= MIN_PORT || port > MAX_PORT)
	throw std::invalid_argument("Port must be between 1024 and 65535");
}

void Validator::pass(const std::string &pass) {
  if (pass.length() != PASSWORD_LENGTH)
	throw std::invalid_argument("pass must be exactly 4 characters");
}
