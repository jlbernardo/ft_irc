#include "ft_irc.hpp"


Client::Client(int fd, Server &server)
	: _fd(fd), _nick(""), _user(""), _name(""), _pass(""),
	_buffer(""), _hostname(""), _identifier(""), _server(server),
	_authenticated(false), _joinedChannels() {
}

Client::~Client() {
}

Client::Client(const Client &copy) : _server(copy._server) {
	*this = copy;
}

Client &Client::operator=(const Client &copy) {
	if (this != &copy) {
		_fd = copy._fd;
		_nick = copy._nick;
		_user = copy._user;
		_name = copy._name;
		_pass = copy._pass;
		_buffer = copy._buffer;
		_hostname = copy._hostname;
		_identifier = copy._identifier;
		_server = copy._server;
		_authenticated = copy._authenticated;
		_joinedChannels = copy._joinedChannels;
	}
	return *this;
}

void Client::set_hostname(int client_socket) {
	struct sockaddr_in addr;
	socklen_t len = sizeof(addr);

	if (getsockname(client_socket, (struct sockaddr *)&addr, &len) == -1) {
		_hostname = "";
		logger.warn("getsockname has failed, thus the name is: " + _hostname);
		return;
	}

	_hostname = inet_ntoa(addr.sin_addr);
	logger.info("The hostname will be: " + _hostname);
}

bool Client::read_into_buffer() {
	static char temp[1024];                                     // to automatically zero-initialize
	ssize_t bytes_read = recv(_fd, temp, sizeof(temp) - 1, 0);  // SEGFAULT HERE

	if (bytes_read <= 0)
		return false;

	_buffer.append(temp, bytes_read);

	return true;
}

int Client::get_fd() const {
	return _fd;
}

const std::string &Client::get_nickname() const {
	return _nick;
}

const std::string &Client::get_username() const {
	return _user;
}

const std::string &Client::get_realname() const {
	return _name;
}

const std::string &Client::get_hostname() const {
	return _hostname;
}

const std::string &Client::get_identifier() const {
	return _identifier;
}

Server &Client::getServer() const {
	return _server;
}

bool Client::is_authenticated() const {
	return _authenticated;
}

bool Client::password_matched(Server &server) const {
	return _pass == server.get_pass();
}

const std::string &Client::get_buffer() const {
	return _buffer;
}

const std::string &Client::get_pass() const {
	return _pass;
}

void Client::set_nickname(const std::string &nickname) {
	_nick = nickname;
}

void Client::set_username(const std::string &username) {
 	_user = username;
}

void Client::set_realname(const std::string &realname) {
	_name = realname;
}

void Client::set_password(const std::string &pass) {
	_pass = pass;
}

void Client::set_authentication(bool status) {
	_authenticated = status;
}

void Client::set_identifier(const std::string &identifier) {
	_identifier = identifier;
}

const std::vector<Channel*> &Client::get_joined_channels() const {
	return _joinedChannels;
}

void Client::add_channel(Channel* channel) {
	_joinedChannels.push_back(channel);
}

void Client::clean_buffer() {
	_buffer.clear();
}

bool Client::buffer_has_linebreak() {
	 return _buffer.find("\n") != std::string::npos;
}
