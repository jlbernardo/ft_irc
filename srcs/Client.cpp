#include "Client.hpp"

// just the client fd constructor for now:
Client::Client(int fd) {
  this->fd = fd;
}

Client::~Client() {}
