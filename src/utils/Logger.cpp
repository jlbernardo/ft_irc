#include "ft_irc.hpp"


Logger::Logger() {
}

Logger::~Logger() {
}

Logger::Logger(const Logger &copy) {
	*this = copy;
}

Logger &Logger::operator=(const Logger &copy) {
	(void)copy;
	return *this;
}

Logger &Logger::getInstance() {
	static Logger instance;
	return instance;
}

void Logger::debug(std::string message) {
	std::string t = timestamp("%d.%m %H:%M:%S");
	std::cout << ORANGE << "[" + t + " DEBUG] " << trim(message) << DFT << std::endl;
}

void Logger::info(std::string message) {
	std::string t = timestamp("%d.%m %H:%M:%S");
	std::cout << "[" + t + "  INFO] " << trim(message) << DFT << std::endl;
}

void Logger::warn(std::string message) {
	std::string t = timestamp("%d.%m %H:%M:%S");
	std::cout << YELLOW << "[" + t + "  WARN] " << trim(message) << DFT << std::endl;
}

void Logger::error(std::string message) {
	std::string t = timestamp("%d.%m %H:%M:%S");
	std::cerr << RED << "[" + t + " ERROR] " << trim(message) << DFT << std::endl;
}

void Logger::fatal(std::string message) {
	std::string t = timestamp("%d.%m %H:%M:%S");
	std::cerr << MAGENTA << "[" + t + " FATAL] " << trim(message) << DFT << std::endl;
}
