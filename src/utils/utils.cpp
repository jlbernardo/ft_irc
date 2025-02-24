#include "ft_irc.hpp"


std::string timestamp(const char *format) {
	std::time_t now = std::time(NULL);
	char buf[100];
	std::strftime(buf, sizeof(buf), format, std::localtime(&now));
	return std::string(buf);
}

std::vector<std::string> split(const std::string &str, char delim) {
    std::vector<std::string> result;
    std::string token;
    std::istringstream tokenStream(str);

    while (std::getline(tokenStream, token, delim)) {
        result.push_back(token);
    }
    return result;
}

std::string trim(const std::string &str) {
	if (str.empty())
		return str;

	// Find first non-whitespace
	std::string::size_type first = str.find_first_not_of(" \t\n\r\f\v");
	if (first == std::string::npos)
		return "";  // String is all whitespace

	// Find last non-whitespace
	std::string::size_type last = str.find_last_not_of(" \t\n\r\f\v");
	return str.substr(first, (last - first + 1));
}
