#include "Client.hpp"

// just the client fd constructor for now:
Client::Client(int fd) {
  _fd = fd;
}

Client::~Client() {}
