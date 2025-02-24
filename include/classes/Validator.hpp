#pragma once
#ifndef VALIDATOR_HPP
# define VALIDATOR_HPP

# include "ft_irc.hpp"

const int MIN_PORT = 1024;
const int MAX_PORT = 65535;


class Validator {
	public:
		Validator();
		~Validator();
		Validator(const Validator &copy);
		Validator &operator=(const Validator &copy);

		void validate(int argc);
		void validate(const char *port_str);
};

#endif
