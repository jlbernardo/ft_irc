#include "Channel.hpp"
#include "ft_irc.h"
#include <map>
#include <ctype.h>

Channel::Channel(const std::string& name, Client* creator) : _name(name) {
	std::string	input;

	if (!creator) {
		errorln("Invalid creator.");
		this->~Channel();
		return ;
	}

	if (!parseChannelName(name)) {
		creator->getServer()->send_message(creator->get_fd(), ERR_BADCHANMASK(name));
		this->~Channel();
		return ;
	}
	
	_members.insert(std::pair<int, Client*>(creator->get_fd(), creator));
	_operators.insert(std::pair<int, Client*>(creator->get_fd(), creator));
	creator->getServer()->addNewChannel(this);
    //Those configs are not made at the channel creation. 

	// input.clear();
	// println("Set channel topic (press ENTER to skip)" << name << ":");
	// std::getline(std::cin, input);
	// if (!input.empty())
	// 	_topic = input;
	
	// input.clear();
	// println("Set channel password (press ENTER to skip):");
	// std::getline(std::cin, input);
	// if (!input.empty()) {
	// 	_hasKey = true;
	// 	_key = input;
	// }

	// input.clear();
	// println("Set channel user limit (press ENTER to skip):");
	// std::getline(std::cin, input);
	// if (!input.empty())
	// 	_userLimit = std::atoi(input.c_str());
}

Channel::~Channel() {
	// for (std::map<int, Client*>::iterator it = _members.begin(); it != _members.end(); it++) {
	// 	delete it->second;
	// }
	// _members.clear();
	// for (std::map<int, Client*>::iterator it = _operators.begin(); it != _operators.end(); it++) {
	// 	delete it->second;
	// }
	// _operators.clear();

	// THIS IS NOT NEEDED, IF WE DELETE THE CLIENT POINTER 
	// IT MEANS HE IS EXCLUDED ALSO FROM THE SERVER, 
	// WHICH IS NOT WHAT WE WANT
}

void Channel::setOperator(Client* client) {
	if (!client) {
		errorln("Invalid client.");
	}
	else if (_members.find(client->get_fd()) == _members.end()) {
		errorln(client->get_username() << " was not found in the members list.");
	}
	else if (isOperator(client)) {
		errorln(client->get_username() << " is already an operator of this channel.");
	}
	else {
		_operators.insert(std::pair<int, Client*>(client->get_fd(), client));
	}
}

bool Channel::isOperator(Client* client) const {
	if (client && _operators.find(client->get_fd()) != _operators.end())
		return true;
	return false;
}

bool Channel::checkChannelModes(char mode) const {
	switch (tolower(mode)) {
		case 'i':
			return _inviteOnly;
		case 't':
			return _topicRestricted;
		case 'k':
			return _hasKey;
		case 'l':
			return _userLimit;
		default:
			errorln("Not a valid channel mode.");
			return false;
	}
}

const std::string& Channel::getName() const {
	return _name;
}

const std::string& Channel::getTopic() const {
	return _topic;
}

const std::map<int, Client*>& Channel::getMembers() const {
	return _members;
}

bool Channel::kickMember(Client* operator_client, Client* target, const std::string& reason) {
	if (!operator_client) {
		errorln("Invalid operator.");
	}
	else if (!target) {
		errorln("Invalid target.");
	}
	else if (!isOperator(operator_client)) {
		errorln("Unable to kick " << target->get_username()
				<< ": you do not have operator privileges.");
	}
	else if (_members.find(target->get_fd()) == _members.end()) {
		errorln(target->get_username() << " is not a member of this channel.");
	}
	else {
		_members.erase(target->get_fd());
		return true;
	}

	return false;
}

bool Channel::inviteMember(Client* operator_client, Client* target) {
	if (!operator_client) {
		errorln("Invalid operator.");
	}
	else if (!target) {
		errorln("Invalid target.");
	}
	else if (!isOperator(operator_client)) {
		errorln("Unable invite members: you do not have operator privileges.");
	}
	else if (_members.find(target->get_fd()) != _members.end()) {
		errorln(target->get_username() << " is already a member of '" << _name << "'.");
	}
	else {
		_members.insert(std::pair<int, Client*>(target->get_fd(), target));
		return true;
	}

	return false;
}

bool Channel::setTopic(Client* operator_client, const std::string& new_topic) {
	if (!operator_client) {
		errorln("Invalid operator.");
	}
	else if (!isOperator(operator_client)) {
		errorln("Unable set topic: you do not have operator privileges.");
	}
	else {
		_topic = new_topic;
		return true;
	}

	return false;
}

bool Channel::parseChannelName(const std::string &name) const
{
	std::string invalidChars = " ^G,";

	size_t pos = name.find_first_of(invalidChars);

	if (name.empty() || (name[0] != '#' && name[0] != '&') || pos != std::string::npos)
		return false;
	return true;
};
