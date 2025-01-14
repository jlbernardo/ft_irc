#pragma once

#include <map>
#include <string>
#include "Client.hpp"
#include "Channel.hpp"

class Server {
 private:
  int _fd;
  int _port;
  std::string _pass;
  fd_set _master_set;
  int _max_fd;
  std::map<std::string, Channel*> _channels;
  std::map<int, Client*> _clients;
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
  friend class SocketsManager;
  int get_fd();
  std::string get_pass();
  void set_pass(const std::string& pass);
  void remove_client(int client_fd);
  // Channel* createChannel(const std::string& name, Client* creator); //need implement
  // void removeChannel(const std::string& name); //need implement
  // Channel* findChannel(const std::string& name); //need implement
  // void handleJoin(Client* client, const std::string& command); //need implement
  // void handlePart(Client* client, const std::string& command); //need implement
  // void handlePrivmsg(Client* client, const std::string& command); //need implement

  void start();
  // void stop();
  void send_error(int client_fd, const std::string& error_code, const std::string& error_message);
  void send_message(int client_fd, const std::string& message);
  void error(int fd, const std::string& msg);
};
