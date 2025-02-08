#include "Channel.hpp"
#include "ft_irc.h"
#include <map>
#include <ctype.h>

Channel::Channel(const std::string& name, Client* creator) : _name(name),
				_topic(""), _inviteOnly(false), _topicRestricted(false),
				_hasKey(false), _userLimit(1024) {
	std::string	input;

	if (!creator) {
		creator->getServer()->send_message(creator->get_fd(), ERR_NONICKNAMEGIVEN());
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
	creator->add_channel(this);
}

Channel::~Channel() {
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

bool Channel::isMember(Client* client) const {
	if (client && _members.find(client->get_fd()) != _members.end())
		return true;
	return false;
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
	(void) reason;
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
		println("Kicking member " << target->get_username() << " for " << reason);
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

bool Channel::addMember(Client* client) {
	if (!client) {
		errorln("Invalid client.");
	}
	else if (_members.find(client->get_fd()) != _members.end()) {
		errorln(client->get_username() << " is already a member of this channel.");
	}
	else {
		_members.insert(std::pair<int, Client*>(client->get_fd(), client));
		return true;
	}

	return false;
}

int Channel::getCurrentMembersCount() {
	return _members.size();
};

int Channel::getUserLimit() {
	return _userLimit;
};
