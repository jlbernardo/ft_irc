#include <map>
#include <string>

#include "Client.hpp"
#include "CommandHandler.hpp"
#include "Commands.hpp"
#include "Server.hpp"
#include "ft_irc.h"

CommandHandler::CommandHandler(std::map<int, Client *> &clients, Server &server) : clients(clients), server(server) {}

void CommandHandler::execute(Commands &command) {
  for (std::list<Command>::iterator it = command.list.begin(); it != command.list.end(); it++) {
    Command temp = *it;
    switch (temp.type) {
      case PASS:
        pass(command, temp.params);
        break;
      case NICK:
        nick(command, temp.params);
        break;
      case USER:
        user(command);
        break;
      default:
        // Handle unknown command
        break;
    }
  }
}

void CommandHandler::pass(Commands &command, const std::string &param) {
  Client &client = command.get_sender();

  // ERR_NEEDMOREPARAMS (461)
  if (param.empty()) {
    server.send_message(client.get_fd(), ERR_NEEDMOREPARAMS(std::string("*")));
    // ERR_ALREADYREGISTERED (462)
  } else if (client.is_authenticated()) {
    server.send_message(client.get_fd(), ERR_ALREADYREGISTERED(std::string("*")));
    // ERR_PASSWDMISMATCH (464)
  } else if (param != server.get_pass()) {
    server.send_message(client.get_fd(), ERR_PASSWDMISMATCH(std::string("*")));
    server.send_message(client.get_fd(), ERROR(std::string("wrong password")));
	command._fatal_error = true;
  } else {
    client.set_authentication(true);
  }
}

void CommandHandler::nick(Commands &command, const std::string &param) {
  Client &client = command.get_sender();

  if (is_nickname_in_use(param)) {
    server.send_error(client.get_fd(), "433", "Nickname is already in use");
    return;
  }
  std::string old_nick = client.get_nickname();
  update_nickname(client, param);
  broadcast_nickname_change(client, old_nick, param);
}

bool CommandHandler::is_nickname_in_use(const std::string &new_nick) {
  for (std::map<int, Client *>::iterator it = clients.begin(); it != clients.end(); ++it) {
    if (it->second->get_nickname() == new_nick) {
      return true;
    }
  }
  return false;
}

void CommandHandler::update_nickname(Client &client, const std::string &new_nick) { client.set_nickname(new_nick); }

void CommandHandler::broadcast_nickname_change(Client &client, const std::string &old_nick,
                                               const std::string &new_nick) {
  std::string message = ":" + old_nick + " NICK " + new_nick + "\r\n";
  // message = std::string(":127.0.0.1") + " 001 " + new_nick + " :Welcome to
  // the IRC server! " + new_nick + "!" + "127.0.0.1" + CRLF; for (std::map<int,
  // Client*>::iterator it = clients.begin(); it != clients.end(); ++it) {
  //   if (it->first != client.get_fd()) {
  //     server.send_message(it->first, message);
  //   }
  // }
  (void)client;
  println("Should have printed: " + message);
  server.send_message(client.get_fd(), message);
  // server.send_message(3, message); // SIGPIPE if hardcoded like that
}

void CommandHandler::user(Commands &command) {
  std::vector<std::string> params = command.get_parameters();
  if (params.size() < 3) {
    // Handle error: not enough parameters
    return;
  }
  Client &client = command.get_sender();
  update_user_info(client, params[0], params[2]);
}

void CommandHandler::update_user_info(Client &client, const std::string &username, const std::string &realname) {
  client.set_username(username);
  client.set_hostname(client.get_fd());  // Pass the sender_fd as the socket descriptor
  client.set_realname(realname);
}

// void CommandHandler::execute_command(const command &command) {}
