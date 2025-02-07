#include "Client.hpp"
#include "Commands.hpp"
#include "Server.hpp"

class CommandsManager {
 public:
  CommandsManager(Server &server);
  void execute(Commands &commands);

 private:
  Server &server;

  void privmsg(Commands &commands, const Command &cmd);
  void join(Commands &commands, const Command &cmd);
  void nick(Commands &commands, const Command &cmd);
  void user(Commands &commands, const Command &cmd);
  // void quit(Commands &commands, const Command &cmd);
  void pass(Commands &commands, const Command &cmd);
  // void mode(Commands &commands, const Command &cmd);
  // void kick(Commands &commands, const Command &cmd);

  void broadcast_message(const std::string &msg, int sender_fd);
  void broadcast_nickname_change(int sender_fd, const std::string &old_nick, const std::string &new_nick);
  void send_welcome_messages(Client &client);
  void update_user_info(Client &client, const std::string &username, const std::string &realname);
  void update_nickname(Client &client, const std::string &new_nick);
  bool is_nickname_in_use(const std::string &new_nick);
};
