#include "Client.hpp"
#include "ft_irc.h"
#include <iostream>

Client::Client(int fd) : _fd(fd), _nick("unknown"), _user("unknown"), _name("unknown"), _authenticated(false), _buffer("") {
  set_hostname(fd);
}

Client::~Client() {}

void Client::set_hostname(int client_socket) {
  struct sockaddr_in addr;
  socklen_t len = sizeof(addr);
  if (getsockname(client_socket, (struct sockaddr*)&addr, &len) == -1) {
    _hostname = "unknown";
    println("getsockname has failed, thus the name is: ");
    printlnnl(_hostname);
    return;
  }
  _hostname = inet_ntoa(addr.sin_addr);
  println("The hostname will be ");
  printlnnl(_hostname);
}

bool Client::read_into_buffer() {
  static char temp[1024];  // to automatically zero-initialize
  ssize_t bytes_read = recv(_fd, temp, sizeof(temp) - 1, 0); // SEGFAULT HERE
  if (bytes_read <= 0) {
    return false;
  }
  _buffer.append(temp, bytes_read);
  println(_buffer);
  return true;
}

int Client::get_fd() const { 
  return _fd; 
}

const std::string& Client::get_nickname() const {
    return _nick; 
}

const std::string& Client::get_username() const {
    return _user;
}

const std::string& Client::get_realname() const {
    return _name;
}

const std::string& Client::get_hostname() const { 
  return _hostname; 
}

bool Client::is_authenticated() const {
   return _authenticated;
}

std::string Client::get_buffer() const {
  return _buffer;
}

void Client::set_nickname(const std::string& nickname) {
  _nick = nickname;
}

void Client::set_username(const std::string& username) {
  _user = username;
}

void Client::set_realname(const std::string& realname) {
  _name = realname;
}

void Client::set_authentication(bool status) {
  _authenticated = status;
}

void Client::clean_buffer() { 
  _buffer.clear();
}

bool Client::buffer_has_linebreak() {
   return _buffer.find("\n") != std::string::npos;
}