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
	_join_order.push_back(creator->get_fd());
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

void Channel::addOperator(Client* client) {
	_operators.insert(std::pair<int, Client*>(client->get_fd(), client));
}

void Channel::removeOperator(Client* client) {
	_operators.erase(client->get_fd());
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

void Channel::inviteMember(Client* target) {
	_invited.insert(std::pair<int, Client*>(target->get_fd(), target));
}

void Channel::consumeInvite(Client* target) {
	_invited.erase(target->get_fd());
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
	std::string invalidChars = " \x07,";

	size_t pos = name.find_first_of(invalidChars);

	if (name.empty() || (name[0] != '#' && name[0] != '&') || pos != std::string::npos)
		return false;
	return true;
};

void Channel::addMember(Client* client) {
	if (!client) {
		logger.error("Invalid client.");
	}
	else if (_members.find(client->get_fd()) != _members.end()) {
		logger.error(client->get_username() + " is already a member of this channel.");
	}
	else {
		_members.insert(std::pair<int, Client*>(client->get_fd(), client));
		_join_order.push_back(client->get_fd());
	}
}

void Channel::removeMember(Client* client) {
	if (!client) {
		logger.error("Invalid client.");
	}
	else if (_members.find(client->get_fd()) == _members.end()) {
		logger.error(client->get_username() + " is not a member of this channel.");
	}
	else {
		_members.erase(client->get_fd());
		_join_order.erase(std::find(_join_order.begin(), _join_order.end(), client->get_fd()));
	}
}

void Channel::broadcast(Client* sender, const std::string& message) {
	if (message.empty()) {
		sender->getServer().send_message(sender->get_fd(), ERR_NOTEXTTOSEND(sender->get_username()));
	}
	else {
		for (std::map<int, Client*>::iterator it = _members.begin(); it != _members.end(); ++it) {
			Client *member = it->second;

			member->getServer().send_message(member->get_fd(), message);
		}
	}
}

int Channel::getCurrentMembersCount() {
	return _members.size();
};

int Channel::getCurrentOperatorsCount() {
	return _operators.size();
};

void Channel::promoteFirstMember() {
	if (!_members.empty()) {
		Client *oldest = _members.find(_join_order.front())->second;
		_operators.insert(std::pair<int, Client*>(oldest->get_fd(), oldest));

		_join_order.pop_front();
		_join_order.push_back(oldest->get_fd());
		
		broadcast(NULL, RPL_MODE("big.little.talk.irc", _name, "+o", oldest->get_nickname()));
	}
};

int Channel::getUserLimit() {
	return _userLimit;
};
