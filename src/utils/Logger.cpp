#include "ft_irc.h"

std::string timestamp(const char *format) {
  std::time_t now = std::time(NULL);
  char buf[100];
  std::strftime(buf, sizeof(buf), format, std::localtime(&now));
  return std::string(buf);
}


Logger::Logger() {
}

Logger::~Logger() {
}

Logger &Logger::getInstance() {
  static Logger instance;
  return instance;
}

void Logger::debug(std::string message) {
  std::string t = timestamp("%d.%m %H:%M:%S");
  std::cout << BLUE << "[" + t + " DEBUG] " << message << DFT << std::endl;
}

void Logger::info(std::string message) {
  std::string t = timestamp("%d.%m %H:%M:%S");
  std::cout << "[" + t + "  INFO] " << message << std::endl;
}

void Logger::warn(std::string message) {
  std::string t = timestamp("%d.%m %H:%M:%S");
  std::cout << YELLOW << "[" + t + "  WARN] " << message << DFT << std::endl;
}

void Logger::error(std::string message) {
  std::string t = timestamp("%d.%m %H:%M:%S");
  std::cerr << RED << "[" + t + " ERROR] " << message << DFT << std::endl;
}

void Logger::fatal(std::string message) {
  std::string t = timestamp("%d.%m %H:%M:%S");
  std::cerr << MAGENTA << "[" + t + " FATAL] " << message << DFT << std::endl;
}
