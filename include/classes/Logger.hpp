#pragma once
#ifndef LOGGER_HPP
# define LOGGER_HPP

# include "ft_irc.hpp"


class Logger {
	private:
		Logger();

	public:
		~Logger();
		Logger(const Logger &copy);
		Logger &operator=(const Logger &copy);

		void debug(std::string message);
		void info(std::string message);
		void warn(std::string message);
		void error(std::string message);
		void fatal(std::string message);

		static Logger &getInstance();
};

#endif
