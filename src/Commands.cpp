#include "ft_irc.hpp"


Commands::Commands(Client &sender, const std::string &raw_input)
		:_sender_fd(sender.get_fd()),  _sender(sender), _fatal_error(false) {
	
	init_command_map();
	init_list_of_commands(raw_input);
}

Commands::~Commands() {
}

Commands::Commands(const Commands &copy) : _sender(copy._sender) {
	*this = copy;
}

Commands &Commands::operator=(const Commands &copy) {
	if (this != &copy) {
		_sender_fd = copy._sender_fd;
		_sender = copy._sender;
		_fatal_error = copy._fatal_error;
		_list = copy._list;
		_command_map = copy._command_map;
	}
	return *this;
}

Client &Commands::get_sender() const {
	return _sender;
}

std::list<Command> &Commands::get_list() {
	return _list;
}

void Commands::init_command_map() {
	_command_map["PRIVMSG"] = PRIVMSG;
	_command_map["JOIN"] = JOIN;
	_command_map["NICK"] = NICK;
	_command_map["USER"] = USER;
	_command_map["QUIT"] = QUIT;
	_command_map["KICK"] = KICK;
	_command_map["PASS"] = PASS;
	_command_map["INVITE"] = INVITE;
	_command_map["TOPIC"] = TOPIC;
	_command_map["MODE"] = MODE;
	_command_map["WHO"] = WHO;
	_command_map["CAP"] = CAP;
	_command_map["PART"] = PART;
	_command_map["NOTICE"] = NOTICE;
	_command_map["AWAY"] = AWAY;

}

void Commands::init_list_of_commands(const std::string &raw_input) {
	std::istringstream iss(raw_input);
	std::vector<std::string> vec;
	vec = separate_multiple_commands(raw_input, "\r\n");

	for (std::vector<std::string>::iterator it = vec.begin(); it != vec.end(); it++) {
		std::istringstream cmd(*it);
		Command cmd_obj;
		cmd >> cmd_obj.command;
		getline(cmd, cmd_obj.params);
		cmd_obj.params = trim(cmd_obj.params);
		to_uppercase(cmd_obj.command);
		cmd_obj.type = find_command_type(cmd_obj.command);

		// Parse parameters
		std::istringstream param_iss(cmd_obj.params);
		std::string param;
		bool found_colon = false;
		while (param_iss >> param) {
			if (param[0] ==  ':') {
				param = param.substr(1);
				found_colon = true;
				std::string remaining;
				std::getline(param_iss, remaining);
				param += remaining;
			}

			cmd_obj.parameters.push_back(param);
			if (found_colon)
				break ;
		}

		this->_list.push_back(cmd_obj);
	}

	// Debug print to verify commands and parameters
	for (std::list<Command>::const_iterator it = _list.begin(); it != _list.end(); ++it) {
		std::string output;

		output = "Command: " + it->command + " | Parameters: ";
		for (std::vector<std::string>::const_iterator param_it = it->parameters.begin(); param_it != it->parameters.end();
				 ++param_it) {
			output += *param_it;
			if (param_it + 1 != it->parameters.end())
				output += ", ";
		}
	 logger.info(output);
	}
}

std::vector<std::string> Commands::separate_multiple_commands(const std::string &input, const std::string &delimiter) {
	std::vector<std::string> result;
	size_t start = 0;

	while (start < input.length()) {
		size_t end = input.find_first_of(delimiter, start);
		if (end == std::string::npos) {
			result.push_back(input.substr(start));
			break;
		}
		result.push_back(input.substr(start, end - start));
		start = end + delimiter.length();
	}
	return result;
}

CommandType Commands::find_command_type(const std::string &cmd) {
	if (_command_map.find(cmd) != _command_map.end()) {
		return _command_map[cmd];
	}
	return UNKNOWN;
}

void Commands::to_uppercase(std::string &str) {
	for (unsigned int i = 0; i < str.length(); i++) {
		str[i] = std::toupper(str[i]);
	}
}
