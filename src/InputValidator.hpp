#pragma once

#include <string>

namespace InputValidator {
  const int MIN_PORT = 1024;
  const int MAX_PORT = 65535;
  const size_t PASSWORD_LENGTH = 4;
  const std::string pass = "89aX";

  void validatePort(int argc, const char *port_str);
  void validatePassword(const std::string &password);
}
