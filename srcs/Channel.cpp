#include "Channel.hpp"
#include "ft_irc.h"
#include <map>
#include <ctype.h>
// #include <utility>

Channel::Channel(const std::string& name, Client* creator) : _name(name) {
	if (creator) {
		_members.insert(std::pair<int, Client*>(creator->get_fd(), creator));
		_operators.insert(std::pair<int, Client*>(creator->get_fd(), creator));
	}
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

	//THIS IS NOT NEEDED, IF WE DELETE THE CLIENT POINTER 
	//IT MEANS HE IS EXCLUDED ALSO FROM THE SERVER, 
	//WHICH IS NOT WHAT WE WANT
}

bool Channel::addMember(Client* client) {
	if (client && _members.find(client->get_fd()) == _members.end()) {
		_members.insert(std::pair<int, Client*>(client->get_fd(), client));
		return true;
	}
	errorln("Already a member!");
	return false;
};

bool Channel::removeMember(Client* client) {
	if (client && _members.find(client->get_fd()) != _members.end()) {
		_members.erase(client->get_fd());
		return true;
	}
	errorln("Member not found.");
	return false;
};

void Channel::setMember(Client* client) {
	if (client)
		_members.insert(std::pair<int, Client*>(client->get_fd(), client));
}

void Channel::setOperator(Client* client) {
	if (client)
		_operators.insert(std::pair<int, Client*>(client->get_fd(), client));
};

bool Channel::isOperator(Client* client) const {
	if (_operators.find(client->get_fd()) != _operators.end()) {
		return true;
	}
	return false;
};

bool Channel::checkChannelModes(char mode) const {
	std::string validChars = "itkl";
	char modeToUpper = toupper(mode);

	if (validChars.find(mode) != std::string::npos) {
		switch (modeToUpper) {
				case 'i':
					return _inviteOnly;
				case 't':
					return _topicRestricted;
				case 'k':
					return _hasKey;
				case 'l':
					return _userLimit;
				default:
					return false;
		}
	}
};

bool Channel::checkUserModes(char mode, Client* client) const {
	char modeToUpper = toupper(mode);
	
	if (modeToUpper != 'O') {
		errorln("Not a valid user mode.");
		return false;
	}
	if (client){
		if (isOperator(client)) {
			_operators.erase(client->get_fd());
		}
		else
			_operators.insert(std::pair<int, Client*>(client->get_fd(), client));
	}

}