#include <string>
#include <map>
#include "Client.hpp"
#include "Parser.hpp"
#include "CommandHandler.hpp"
#include "Server.hpp"
#include "ft_irc.h"

CommandHandler::CommandHandler(std::map<int, Client*> &clients, Server &server) : clients(clients), server(server) {}

void CommandHandler::handle_command(std::vector<CommandEntry> command_entries) {
  //make a for loop here to iterate over the command_entries and feed the switch 
  //passing command_entries.type as the case and command_entries.param as the argument for the functions
  // e.g. in the commands below
for (std::vector<CommandEntry>::iterator it = command_entries.begin(); it != command_entries.end(); it++){
    CommandEntry temp = *it;
    switch (temp.type) {
      case NICK:
        nick(temp.params);
        //nick(command_entries[i].params)
        //ps: we have to adapt the nick function to work with the command_entries struct.
        break;
      case USER:
        user(temp.params);
        break;
      default:
        // Handle unknown command
        break;
    }
  }
}

void CommandHandler::nick(const Parser &parser) {
  Client &client = parser.get_sender();
  std::string new_nick = parser.get_target();

  if (is_nickname_in_use(new_nick)) {
    server.send_error(client.get_fd(), "433", "Nickname is already in use");
    return;
  }
  std::string old_nick = client.get_nickname();
  update_nickname(client, new_nick);
  broadcast_nickname_change(client, old_nick, new_nick);
}

bool CommandHandler::is_nickname_in_use(const std::string &new_nick) {
  for (std::map<int, Client*>::iterator it = clients.begin(); it != clients.end(); ++it) {
    if (it->second->get_nickname() == new_nick) {
      return true;
    }
  }
  return false;
}

void CommandHandler::update_nickname(Client &client, const std::string &new_nick) {
  client.set_nickname(new_nick);
}

void CommandHandler::broadcast_nickname_change(Client &client, const std::string &old_nick, const std::string &new_nick) {
  std::string message = ":" + old_nick + " NICK " + new_nick + "\r\n";
  // message = std::string(":127.0.0.1") + " 001 " + new_nick + " :Welcome to the IRC server! " + new_nick + "!" + "127.0.0.1" + CRLF;
  for (std::map<int, Client*>::iterator it = clients.begin(); it != clients.end(); ++it) {
    if (it->first != client.get_fd()) {
      server.send_message(it->first, message);
    }
  }
}

void CommandHandler::user(const Parser &parser) {
  std::vector<std::string> params = parser.get_parameters();
  if (params.size() < 3) {
    // Handle error: not enough parameters
    return;
  }
  Client &client = parser.get_sender();
  update_user_info(client, params[0], params[2]);
}

void CommandHandler::update_user_info(Client &client, const std::string &username, const std::string &realname) {
  client.set_username(username);
  client.set_hostname(client.get_fd()); // Pass the sender_fd as the socket descriptor
  client.set_realname(realname);
}

// void CommandHandler::execute_command(const Parser &parser) {}
