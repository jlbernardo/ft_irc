#pragma once
#ifndef LOGGER_HPP
# define LOGGER_HPP

# include "ft_irc.h"

class Logger {
  private:
    Logger();

  public:
    ~Logger();

    void debug(std::string message);
    void info(std::string message);
    void warn(std::string message);
    void error(std::string message);
    void fatal(std::string message);

    static Logger &getInstance();
};

std::string timestamp() {
  std::time_t now = std::time(NULL);
  char buf[100];
  std::strftime(buf, sizeof(buf), "%d.%m %H:%M:%S", std::localtime(&now));
  return std::string(buf);
}

#endif
