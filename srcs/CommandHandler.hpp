#include "Client.hpp"
#include "Commands.hpp"
#include "Server.hpp"
#include <map>

class CommandHandler {
private:
  std::map<int, Client *> &clients;

public:
  CommandHandler(std::map<int, Client *> &clients, Server &server);
  Server &server;
  void execute_command(const Commands &parser);
  void pass(Commands &parser, const std::string &password);
  void nick(Commands &parser, const std::string &new_nick);
  void user(Commands &parser);
  void execute(Commands &parser);
  bool is_nickname_in_use(const std::string &new_nick);
  void update_nickname(Client &client, const std::string &new_nick);
  void broadcast_nickname_change(Client &client, const std::string &old_nick, const std::string &new_nick);
  void update_user_info(Client &client, const std::string &username, const std::string &realname);
};
