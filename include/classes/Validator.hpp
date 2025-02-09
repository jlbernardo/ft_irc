#pragma once
#ifndef VALIDATOR_HPP
# define VALIDATOR_HPP

# include <string>

class Validator {
  private:
    const int MIN_PORT = 1024;
    const int MAX_PORT = 65535;
    const size_t PASSWORD_LENGTH = 4;
    const std::string pass = "89aX";

  public:
    Validator();
    ~Validator();

    void args(int argc);
    void port(const char *port_str);
    void pass(const std::string &pass);

    static Validator &getInstance();
};

#endif
