#pragma once
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <string>

class Client {
 private:
  int fd;
  std::string nick;
  std::string user;
  std::string name;
  std::string hostname;
  bool authenticated;
  std::string buffer;

 public:
  Client(int fd);
  ~Client() {}

  void clean_buffer() { buffer.clear(); }
  bool buffer_has_linebreak() { return buffer.find("\n") != std::string::npos; }
  bool read_into_buffer();

  // accessors:
  int get_fd() const { return fd; }
  const std::string& get_nickname() const { return nick; }
  const std::string& get_username() const { return user; }
  const std::string& get_realname() const { return name; }
  const std::string& get_hostname() const { return hostname; }
  void set_hostname(int client_socket);
  bool is_authenticated() const { return authenticated; }
  std::string get_buffer() const { return buffer; }
  void set_nickname(const std::string& nickname) { nick = nickname; }
  void set_username(const std::string& username) { user = username; }
  void set_realname(const std::string& realname) { name = realname; }
  void set_authenticated(bool status) { authenticated = status; }
};
