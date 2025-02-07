#pragma once

#include <map>
#include <string>
#include "Channel.hpp"
#include "Client.hpp"
#include <queue>

typedef std::map<int, std::queue<std::string> > MessageQueueMap;
typedef std::map<int, Client*> ClientMap;

class Channel;

class Server {
 private:
  int _fd;
  int _port;
  std::string _pass;
  fd_set _master_set;
  int _max_fd;
  std::map<std::string, Channel*> _channels;
  ClientMap _clients;
  MessageQueueMap _message_queues;
  struct sockaddr_in _server_addr;
  static void signal_handler(int signum);

  void register_signals();
  void initialize_socket();
  void setup_server();
  void add_new_client_to_master_set();
  void handle_client_message(int client_fd);

 public:
  Server(int port, const std::string& pass);
  ~Server();
  static volatile __sig_atomic_t terminate;
  int get_fd();
  std::string get_pass();
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
