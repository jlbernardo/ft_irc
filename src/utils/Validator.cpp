#include "ft_irc.hpp"

Validator::Validator() {
}

Validator::~Validator() {
}

void Validator::validate(int argc) {
  if (argc != 3) {
	throw std::invalid_argument("Invalid number of arguments");
  }
}

void Validator::validate(const char* port_str) {
  for (const char* c = port_str; *c != '\0'; ++c) {
	if (!std::isdigit(*c))
	  throw std::invalid_argument("Port must be a number");
  }

  int port = std::atoi(port_str);
  if (port <= MIN_PORT || port > MAX_PORT)
	throw std::invalid_argument("Port must be between 1024 and 65535");
}
