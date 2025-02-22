#include "ft_irc.hpp"


Channel::Channel(const std::string& name, Client* creator) : _name(name),
				_topic(""), _key(""), _inviteOnly(false), _topicRestricted(false),
				_hasKey(false), _userLimit(0) {
	std::string	input;

	if (!creator) {
		creator->getServer().send_message(creator->get_fd(), ERR_NONICKNAMEGIVEN());
		return ;
	}

	if (!parseChannelName(name)) {
		creator->getServer().send_message(creator->get_fd(), ERR_BADCHANMASK(name));
		return ;
	}

	_members.insert(std::pair<int, Client*>(creator->get_fd(), creator));
	_operators.insert(std::pair<int, Client*>(creator->get_fd(), creator));
}

Channel::~Channel() {
}

Channel& Channel::operator=(const Channel &copy) {
	if (this != &copy) {
		_name = copy._name;
		_topic = copy._topic;
		_key = copy._key;
		_members = copy._members;
		_operators = copy._operators;
		_inviteOnly = copy._inviteOnly;
		_topicRestricted = copy._topicRestricted;
		_hasKey = copy._hasKey;
		_userLimit = copy._userLimit;
	}
	return *this;
}

Channel::Channel(const Channel &copy) {
	*this = copy;
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

bool Channel::isInvited(Client* client) const {
	if (client && _invited.find(client->get_fd()) != _invited.end())
		return true;
	return false;
}

std::string Channel::getModes() const {
	std::string modes = "+";

	if (_inviteOnly)
		modes += "i";
	if (_topicRestricted)
		modes += "t";
	if (_hasKey)
		modes += "k";
	if (_userLimit)
		modes += "l";
	if (modes.length() == 1)
		modes = "";

	return modes;
}

std::string Channel::getModeParams() const {
	std::string params = "";

	if (_hasKey)
		params += _key + " ";
	if (_userLimit)
		params += to_string(_userLimit) + " ";

	return params;
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

void Channel::setInviteOnly(bool value) {
	_inviteOnly = value;
}

void Channel::setTopicRestricted(bool value) {
	_topicRestricted = value;
}

void Channel::setKey(const std::string& key) {
	_key = key;
	_hasKey = !key.empty();
}

void Channel::setUserLimit(size_t limit) {
	_userLimit = limit;
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

const std::string& Channel::getKey() const {
	return _key;
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
