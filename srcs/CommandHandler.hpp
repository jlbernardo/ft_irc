#include "Client.hpp"
#include "Parser.hpp"
#include "Server.hpp"
#include <map>

class CommandHandler {
private:
  std::map<int, Client *> &clients;
  Server &server;

public:
  CommandHandler(std::map<int, Client *> &clients, Server &server);

  void execute_command(const Parser &parser);
  void nick(const Parser &parser, const std::string &new_nick);
  void user(const Parser &parser);
  void handle_command(Parser &parser);
  bool is_nickname_in_use(const std::string &new_nick);
  void update_nickname(Client &client, const std::string &new_nick);
  void broadcast_nickname_change(Client &client, const std::string &old_nick, const std::string &new_nick);
  void update_user_info(Client &client, const std::string &username, const std::string &realname);
};
