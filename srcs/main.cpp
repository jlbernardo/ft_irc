#include <cstdlib>
#include <iostream>

#include "Server.hpp"

static void validate_port(int argc, const char* port_str);
static void print_usage(const char* program_name);

int main(int argc, char* argv[]) {
  try {
    validate_port(argc, argv[1]);
    int port = std::atoi(argv[1]);

    Server irc_server(port);
    irc_server.start();
  } catch (const std::invalid_argument& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    print_usage(argv[0]);
    return 1;
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }
}

static void validate_port(int argc, const char* port_str) {
  if (argc != 2) {
    throw std::invalid_argument("Invalid number of arguments");
  }
  for (const char* c = port_str; *c != '\0'; ++c) {
    if (!std::isdigit(*c)) {
      throw std::invalid_argument("Port must be a number");
    }
  }
  int port = std::atoi(port_str);
  if (port <= 1024 || port > 65535) {
    throw std::invalid_argument("Port must be between 1024 and 65535");
  }
}

static void print_usage(const char* program_name) {
  std::cerr << "Usage: " << program_name << " <port>" << std::endl;
  std::cerr << "Port must be between 1024 and 65535" << std::endl;
}
