#include "ft_irc.h"


Channel::Channel(const std::string& name, Client* creator) : _name(name),
				_topic(""), _inviteOnly(false), _topicRestricted(false),
				_hasKey(false), _userLimit(1024) {
	std::string	input;

	if (!creator) {
		creator->getServer().send_message(creator->get_fd(), ERR_NONICKNAMEGIVEN());
		this->~Channel();
		return ;
	}

	if (!parseChannelName(name)) {
		creator->getServer().send_message(creator->get_fd(), ERR_BADCHANMASK(name));
		this->~Channel();
		return ;
	}

	_members.insert(std::pair<int, Client*>(creator->get_fd(), creator));
	_operators.insert(std::pair<int, Client*>(creator->get_fd(), creator));
	creator->getServer().addNewChannel(this);
	creator->add_channel(this);
}

Channel::~Channel() {
}

void Channel::setOperator(Client* client) {
	if (!client) {
		client->getServer().send_message(client->get_fd(),
		ERR_NEEDMOREPARAMS("CLIENT"));
	}
	else if (_members.find(client->get_fd()) == _members.end()) {
		client->getServer().send_message(client->get_fd(),
		ERR_USERNOTINCHANNEL(client->get_username(), client->get_nickname(), _name));
	}
	else if (isOperator(client)) {
		client->getServer().send_message(client->get_fd(),
		ERROR("Already an operator of this channel."));
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

bool Channel::mode(char mode) const {
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
			logger.error("Not a valid channel mode.");
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

bool Channel::kickMember(Client* oper, Client* target, const std::string& reason) {
	if (!oper) {
		logger.error("Invalid operator.");
	}
	else if (!target) {
		oper->getServer().send_message(oper->get_fd(),
		ERR_NEEDMOREPARAMS("TARGET"));
	}
	else if (!isOperator(oper)) {
		oper->getServer().send_message(oper->get_fd(),
		ERR_CHANOPRIVSNEEDED(oper->get_username(), _name));
	}
	else if (_members.find(target->get_fd()) == _members.end()) {
		oper->getServer().send_message(oper->get_fd(),
		ERR_USERNOTINCHANNEL(oper->get_username(), target->get_username(), _name));
	}
	else {
		_members.erase(target->get_fd());
		if (reason.empty()) {
			broadcast(NULL, RPL_KICKNOREASON(oper->get_nickname(), oper->get_username(),
					  _name, target->get_username()));
		}
		else {
			broadcast(NULL, RPL_KICKREASON(oper->get_nickname(), oper->get_username(),
					  _name, target->get_username(), reason));
		}
		return true;
	}
	return false;
}

bool Channel::inviteMember(Client* oper, Client* target) {
	if (!oper) {
		logger.error("Invalid operator.");
	}
	else if (!target) {
		oper->getServer().send_message(oper->get_fd(),
		ERR_NEEDMOREPARAMS("TARGET"));
	}
	else if (!isOperator(oper)) {
		oper->getServer().send_message(oper->get_fd(),
		ERR_CHANOPRIVSNEEDED(oper->get_username(), _name));
	}
	else if (_members.find(target->get_fd()) != _members.end()) {
		oper->getServer().send_message(oper->get_fd(),
		ERR_USERONCHANNEL(target->get_nickname(), _name));
	}
	else {
		oper->getServer().send_message(oper->get_fd(),
			RPL_INVITING(oper->get_nickname(), target->get_nickname(), _name));
		target->getServer().send_message(target->get_fd(),
			RPL_INVITEMSG(oper->get_nickname(), oper->get_username(), target->get_username(), _name));
		
		return true;
	}

	return false;
}

bool Channel::setTopic(Client* oper, const std::string& new_topic) {
	if (!oper) {
		logger.error("Invalid operator.");
	}
	else if (!isOperator(oper)) {
		oper->getServer().send_message(oper->get_fd(),
		ERR_CHANOPRIVSNEEDED(oper->get_username(), _name));
	}
	else {
		_topic = new_topic;
		broadcast(NULL, RPL_TOPIC(oper->get_nickname() + "has changed the topic for",
				  _name, new_topic));
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
		logger.error("Invalid client.");
	}
	else if (_members.find(client->get_fd()) != _members.end()) {
		logger.error(client->get_username() + " is already a member of this channel.");
	}
	else {
		_members.insert(std::pair<int, Client*>(client->get_fd(), client));
		return true;
	}

	return false;
}

void Channel::broadcast(Client* sender, const std::string& message) {
	if (message.empty()) {
		sender->getServer().send_message(sender->get_fd(),
		ERR_NOTEXTTOSEND(sender->get_username()));
	}
	else {
		for (std::map<int, Client*>::iterator it = _members.begin(); it != _members.end(); ++it) {
			Client *member = it->second;

			if (member->get_fd() == sender->get_fd())
				continue ;

			member->getServer().send_message(member->get_fd(), message);
		}
	}
}

int Channel::getCurrentMembersCount() {
	return _members.size();
};

int Channel::getUserLimit() {
	return _userLimit;
};
