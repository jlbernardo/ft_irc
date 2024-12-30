#include <map>
#include "Client.hpp"
#include "Parser.hpp"

class CommandHandler {
 private:
  std::map<int, Client*> &clients;

 public:
  CommandHandler(std::map<int, Client*> &clients);

  void execute_command(const Parser &parser);
  void nick(const Parser &parser);
  void user(const Parser &parser);
};
