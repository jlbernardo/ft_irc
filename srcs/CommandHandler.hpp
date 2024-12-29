#include <map>
#include "Client.hpp"
#include "Parser.hpp"

class CommandHandler {
 private:
  std::map<int, Client*> &clients;

 public:
  CommandHandler(std::map<int, Client*> &clients);

  void handle_command(const Parser &parser);
  void handle_nick(const Parser &parser);
  void handle_user(const Parser &parser);
};
