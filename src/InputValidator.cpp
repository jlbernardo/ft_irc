#include <cstdlib>
#include <stdexcept>
#include "InputValidator.hpp"

namespace InputValidator {
  void validatePort(int argc, const char* port_str) {
	if (argc != 3) {
	  throw std::invalid_argument("Invalid number of arguments");
	}
	for (const char* c = port_str; *c != '\0'; ++c) {
	  if (!std::isdigit(*c)) {
		throw std::invalid_argument("Port must be a number");
	  }
	}
	int port = std::atoi(port_str);
	if (port <= MIN_PORT || port > MAX_PORT) {
	  throw std::invalid_argument("Port must be between 1024 and 65535");
	}
  }

  void validatePassword(const std::string& password) {
	if (password.length() != PASSWORD_LENGTH) {
	  throw std::invalid_argument("\033[31mPassword must be exactly 4 characters\033[0m");
	}
  }
}
