#pragma once
#ifndef SERVER_HPP
# define SERVER_HPP

# include "ft_irc.h"

typedef std::map<int, std::queue<std::string> > MessageQueueMap;

template <typename T> std::string to_string(T value) {
  std::ostringstream oss;
  oss << value;
  return oss.str();
}


class Server {
 private:
  int _fd;
  int _port;
  int _max_fd;
  std::string _startup_date;
  std::string _pass;
  fd_set _master_set;
  std::map<std::string, Channel*> _channels;
  std::map<int, Client *> _clients;
  MessageQueueMap _message_queues;
  struct sockaddr_in _server_addr;

  static void signal_handler(int signum);

  void register_signals();
  void initialize_socket();
  void setup_server();
  void add_new_client_to_master_set();
  void handle_client_message(int client_fd);

 public:
  static volatile __sig_atomic_t terminate;

  Server(int port, const std::string& pass);
  ~Server();

  int get_fd();
  std::string &get_pass();
  std::string &get_startup_date();
  std::map<std::string, Channel*> &get_channels();
  std::map<int, Client *> &get_clients();
  MessageQueueMap &get_message_queues();
  void set_pass(const std::string& pass);
  void remove_client(int client_fd);

  void start();
  void send_error(int client_fd, const std::string& error_code, const std::string& error_message);
  void send_message(int client_fd, const std::string& message);
  void error(int fd, const std::string& msg);
  void addNewChannel(Channel* new_channel);
  bool checkForChannel(const std::string& channel_name);

  // Manager classes:
  friend class SocketsManager;
  friend class CommandsManager;
};

#endif
