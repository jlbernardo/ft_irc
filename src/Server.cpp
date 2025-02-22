#include "ft_irc.hpp"


volatile sig_atomic_t Server::terminate = 0;

Server::Server(int port, const std::string& pass) : _fd(0), _port(port),
                _max_fd(0), _pass(pass), _master_set(), _startup_date(""),
                _message_queues(), _server_addr(), _clients(), _channels() {

  register_signals();
  initialize_socket();
  setup_server();
}

Server::~Server() {
 logger.info("Shutting down server");

  for (std::map<std::string, Channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it) {
    delete it->second;
  }

  for (std::map<int, Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
    delete it->second;
    close(it->first);
  }

  close(_fd);
}

Server::Server(const Server &copy) {
  *this = copy;
}

Server &Server::operator=(const Server &copy) {
  if (this != &copy) {
    _fd = copy._fd;
    _port = copy._port;
    _max_fd = copy._max_fd;
    _pass = copy._pass;
    _master_set = copy._master_set;
    _startup_date = copy._startup_date;
    _message_queues = copy._message_queues;
    _server_addr = copy._server_addr;
    _clients = copy._clients;
    _channels = copy._channels;
  }
  return *this;
}

void Server::register_signals() {
  int signals[6] = {
    SIGINT, SIGTERM, SIGQUIT, SIGHUP, SIGPIPE, SIGABRT
  };
  
  for (size_t i = 0; i < 6; ++i)
    signal(signals[i], signal_handler);
}

void Server::signal_handler(int signum) {
  terminate = signum;
}

void Server::initialize_socket() {
  _fd = socket(AF_INET, SOCK_STREAM, 0);

  if (_fd < 0)
    throw std::runtime_error("Failed to create socket");

  memset(&_server_addr, 0, sizeof(_server_addr));
  _server_addr.sin_family = AF_INET;
  _server_addr.sin_addr.s_addr = INADDR_ANY;
  _server_addr.sin_port = htons(_port);
}

void Server::setup_server() {
  int opt = 1;
  
  setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  if (bind(_fd, (struct sockaddr *)&_server_addr, sizeof(_server_addr)) < 0) {
    close(_fd);
    throw std::runtime_error("Failed to bind socket");
  }
  else if (listen(_fd, SOMAXCONN) < 0) {
    close(_fd);
    throw std::runtime_error("Failed to listen on socket");
  }

  FD_ZERO(&_master_set);
  FD_SET(_fd, &_master_set);
  _max_fd = _fd;
}

void Server::start() {
  _startup_date = timestamp("%b %d, %Y at %H:%M:%S");
  logger.info("Server started on port " + to_string(_port) + " at " + _startup_date);

  SocketsManager manager(*this);
  while (true) {
    manager.add_new_sockets_from_masterset_to_read_write();
    manager.io_multiplexing();
    for (int fd = 0; fd <= _max_fd; fd++) {
      manager.socket_read(fd);
      manager.socket_write(fd);
    }
  }
}

void Server::add_new_client_to_master_set() {
  struct sockaddr_in client_addr;
  socklen_t client_addr_len = sizeof(client_addr);
  int client_fd = accept(_fd, (struct sockaddr *)&client_addr, &client_addr_len);

  if (client_fd < 0) {
    logger.error("Failed to accept connection");
    return;
  }

  FD_SET(client_fd, &_master_set);
  if (client_fd > _max_fd)
    _max_fd = client_fd;

  _clients[client_fd] = new Client(client_fd, *this);

#ifdef TEST
  std::stringstream str;
  str << "Connected to server on socket " << client_fd << ": ";
  send(client_fd, str.str().c_str(), str.str().length(), 0);
#endif
}

void Server::remove_client(int client_fd) {
  FD_CLR(client_fd, &_master_set);
  close(client_fd);
  delete _clients[client_fd];
  _clients.erase(client_fd);
}

void Server::send_error(int client_fd, const std::string &error_code, const std::string &error_message) {
  std::string message = ":" + to_string(client_fd) + " " + error_code + " " + error_message + "\r\n";
  send_message(client_fd, message);
}

void Server::send_message(int client_fd, const std::string &message) {
  logger.info("Sending message to client " + to_string(client_fd) + ":\n " + message);

  if (send(client_fd, message.c_str(), message.length(), 0) == -1)
    logger.error("Error sending message to client " + to_string(client_fd));
}

int Server::get_fd() {
  return _fd;
}

std::string &Server::get_pass() {
  return _pass;
}

std::string &Server::get_startup_date() {
  return _startup_date;
}

std::map<std::string, Channel*> &Server::get_channels() {
  return _channels;
}

std::map<int, Client *> &Server::get_clients() {
  return _clients;
}

MessageQueueMap &Server::get_message_queues() {
  return _message_queues;
}

void Server::set_pass(const std::string& pass) {
  _pass = pass;
}

void Server::addChannel(Channel* new_channel) {
    _channels[new_channel->getName()] = new_channel;
}

bool Server::channelExists(const std::string& channel_name) {
    if (_channels.find(channel_name) == _channels.end())
        return false;
    return true;
}

bool Server::clientExists(std::string client_name) {
    for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        if (it->second->get_nickname() == client_name)
            return true;
    }
    return false;
}

Client &Server::getClient(std::string client_name) {
    for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        if (it->second->get_nickname() == client_name)
            return *it->second;
    }
    throw std::runtime_error("Client not found");
}

// void Server::stop() {}

// void Server::handlePrivmsg(Client* client, const std::string& command) {} //need implement
// void Server::handlePart(Client* client, const std::string& command){} //need implement
// Channel* Server::createChannel(const std::string& name, Client* creator){} //need implement
// void Server::removeChannel(const std::string& name) {} //need implement
// Channel* Server::findChannel(const std::string& name) {} //need implement
// void Server::handleJoin(Client* client, const std::string& command) {} //need implement
// void Server::handle_client_message(int client_fd) {}
