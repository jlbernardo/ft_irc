#include <map>
#include <queue>

#include "Client.hpp"
#include "Commands.hpp"
#include "Server.hpp"

class CommandHandler {
 private:
  std::map<int, Client *> &clients;
  void broadcast_message(const Commands &message, int sender_fd,
                         std::map<int, std::queue<std::string> > &message_queues, std::map<int, Client *> _clients);
  void send_welcome_messages(Client &client);

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
  void broadcast_nickname_change(const std::string &old_nick, const std::string &new_nick);
  void update_user_info(Client &client, const std::string &username, const std::string &realname);
  bool is_valid_nickname(const std::string &nickname);
  bool is_valid_username(const std::string &username);
  bool is_nick_in_use(const std::string &new_nick);
};
