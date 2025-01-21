#include <map>
#include <queue>
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
      case CAP:
          break;
      default:
        break;
    }
  }
}

void CommandHandler::pass(Commands &command, const std::string &pass) {
  Client &client = command.get_sender();
  if (pass.empty()) {
    server.send_message(client.get_fd(), ERR_NEEDMOREPARAMS(command.list.front().command));
  } else if (client.password_matched(server)) {
    server.send_message(client.get_fd(), ERR_ALREADYREGISTERED(command.get_sender().get_username()));
  } else if (pass != server.get_pass()) {
    server.send_message(client.get_fd(), ERR_PASSWDMISMATCH());
    server.send_message(client.get_fd(), ERROR(std::string("wrong password")));
    command._fatal_error = true;
  } else {
    client.set_authentication(true);
    send_welcome_messages(client);
  }
}

void CommandHandler::nick(Commands &command, const std::string &param) {
  Client &client = command.get_sender();
  if (param.empty()) {
    server.send_message(client.get_fd(), ERR_NONICKNAMEGIVEN());
    return;
  } else if (is_nickname_in_use(param)) {
    server.send_message(client.get_fd(), ERR_NICKNAMEINUSE(param));
    return;
  }
  std::string old_nick = client.get_nickname();
  update_nickname(client, param);
  broadcast_nickname_change(old_nick, param);
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

void CommandHandler::broadcast_message(const Commands &cmd, int sender_fd,
                                       std::map<int, std::queue<std::string> > &message_queues,
                                       std::map<int, Client *> clients) {
  std::string formatted = cmd.format_command();
  for (std::map<int, Client *>::iterator it = clients.begin(); it != clients.end(); ++it) {
    int client_fd = it->first;
    if (client_fd != sender_fd) {
      message_queues[client_fd].push(formatted);
    }
  }
}

void CommandHandler::broadcast_nickname_change(const std::string &old_nick, const std::string &new_nick) {
  std::string cmd = old_nick + " changed his nickname to " + new_nick + "\r\n";
  // cmd = std::string(":127.0.0.1") + " 001 " + new_nick + " :Welcome to
  // the IRC server! " + new_nick + "!" + "127.0.0.1" + CRLF; for (std::map<int,
  // Client*>::iterator it = clients.begin(); it != clients.end(); ++it) {
  //   if (it->first != client.get_fd()) {
  //     server.send_message(it->first, cmd);
  //   }
  // }
  // println("Should have printed: " + cmd);
  // server.send_message(3, cmd); // SIGPIPE if hardcoded like that
}

void CommandHandler::user(Commands &command) {
  std::vector<std::string> params = command.get_parameters();
  if (params.size() < 3) {
    // Handle error: not enough parameters
    return;
  }
  Client &client = command.get_sender();
  update_user_info(client, params[0], params[2]);
  std::cout << "Client username is: " << client.get_username() << std::endl;
}

void CommandHandler::send_welcome_messages(Client &client) {
  if (!client.is_authenticated()) return;

  server.send_message(client.get_fd(), RPL_WELCOME(client.get_nickname(), client.get_username()));
  server.send_message(client.get_fd(), RPL_YOURHOST(client.get_nickname()));
  server.send_message(client.get_fd(), RPL_CREATED(client.get_nickname()));
  server.send_message(client.get_fd(), RPL_MYINFO(client.get_nickname(), "", ""));
}

void CommandHandler::update_user_info(Client &client, const std::string &username, const std::string &realname) {
  client.set_username(username);
  client.set_hostname(client.get_fd());  // Pass the sender_fd as the socket descriptor
  client.set_realname(realname);
}

// void CommandHandler::execute_command(const command &command) {}
