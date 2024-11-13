#pragma once
#include <sys/socket.h>

#include <string>

class Client {
 private:
  int fd;
  std::string nick;
  std::string user;
  std::string name;
  bool authenticated;
  std::string buffer;

 public:
  Client(int fd) : fd(fd), buffer("") {};
  ~Client() {};

  void clean_buffer() { buffer.clear(); }
  bool buffer_has_linebreak() { return buffer.find("\n") != std::string::npos; }
  bool read_into_buffer();

  // accessors:
  int get_fd() const { return fd; }
  const std::string& get_nickname() const { return nick; }
  const std::string& get_username() const { return user; }
  const std::string& get_realname() const { return name; }
  bool is_authenticated() const { return authenticated; }
  std::string get_buffer() const { return buffer; }
  void set_nickname(const std::string& nickname) { nick = nickname; }
  void set_username(const std::string& username) { user = username; }
  void set_realname(const std::string& realname) { name = realname; }
  void set_authenticated(bool status) { authenticated = status; }
};
