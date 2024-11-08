#include <cstdlib>
#include <iostream>

#include "InputValidator.hpp"
#include "Server.hpp"

static void print_usage(const char* program_name);

int main(int argc, char* argv[]) {
  try {
    InputValidator::validatePort(argc, argv[1]);
    InputValidator::validatePassword(argv[2]);

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

static void print_usage(const char* program_name) {
  std::cerr << "Usage: " << program_name << " <port> <password>" << std::endl;
  std::cerr << "Port must be between 1024 and 65535" << std::endl;
  std::cerr << "Password must be 4 characters" << std::endl;
}