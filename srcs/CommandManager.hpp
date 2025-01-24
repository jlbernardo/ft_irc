#include "Client.hpp"
#include "Commands.hpp"
#include "Server.hpp"

class CommandManager {
 private:
  Server &server;
  void broadcast_message(const std::string &message, int sender_fd);
  void broadcast_nickname_change(int sender_fd, const std::string &old_nick, const std::string &new_nick);
  void send_welcome_messages(Client &client);

 public:
  CommandManager(Server &server);
  void execute_command(const Commands &parser);
  void pass(Commands &parser, const std::string &password);
  void nick(Commands &parser, const std::string &new_nick);
  void user(Commands &parser);
  void execute(Commands &parser);
  bool is_nickname_in_use(const std::string &new_nick);
  void update_nickname(Client &client, const std::string &new_nick);
  void update_user_info(Client &client, const std::string &username, const std::string &realname);
  bool is_valid_nickname(const std::string &nickname);
  bool is_valid_username(const std::string &username);
  bool is_nick_in_use(const std::string &new_nick);
};
