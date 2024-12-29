#include <string>
#include <map>
#include "Client.hpp"
#include "Parser.hpp"
#include "CommandHandler.hpp"

CommandHandler::CommandHandler(std::map<int, Client> &clients) : clients(clients) {}

void CommandHandler::handle_command(const Parser &parser) {
  switch (parser.get_command_type()) {
    case NICK:
      handle_nick(parser);
      break;
    case USER:
      handle_user(parser);
      break;
    // Add other command handlers here
    default:
      // Handle unknown command
      break;
  }
}

void CommandHandler::handle_nick(const Parser &parser) {
  std::string new_nick = parser.get_target();
  int sender_fd = parser.get_sender();
  Client &client = clients[sender_fd];
  client.set_nickname(new_nick);
  // Broadcast the nickname change to other clients if necessary
}

void CommandHandler::handle_user(const Parser &parser) {
  std::vector<std::string> params = parser.get_parameters();
  if (params.size() < 3) {
    // Handle error: not enough parameters
    return;
  }
  std::string username = params[0];
  std::string realname = params[2];
  int sender_fd = parser.get_sender();
  Client &client = clients[sender_fd];
  client.set_username(username);
  client.set_hostname(sender_fd); // Pass the sender_fd as the socket descriptor
  client.set_realname(realname);
  // Perform additional actions if necessary
}