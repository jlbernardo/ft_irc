#include <string>

#include "Client.hpp"
#include "CommandManager.hpp"
#include "ft_irc.h"

CommandManager::CommandManager(Server &server) : server(server) {}

void CommandManager::execute(Commands &command) {
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

void CommandManager::pass(Commands &command, const std::string &pass) {
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

void CommandManager::nick(Commands &command, const std::string &param) {
  Client &sender = command.get_sender();
  if (param.empty()) {
    server.send_message(sender.get_fd(), ERR_NONICKNAMEGIVEN());
    return;
  } else if (is_nickname_in_use(param)) {
    server.send_message(sender.get_fd(), ERR_NICKNAMEINUSE(param));
    return;
  }
  std::string old_nick = sender.get_nickname();
  update_nickname(sender, param);
  broadcast_nickname_change(sender.get_fd(), old_nick, param);
}

bool CommandManager::is_nickname_in_use(const std::string &new_nick) {
  for (std::map<int, Client *>::iterator it = server._clients.begin(); it != server._clients.end(); ++it) {
    if (it->second->get_nickname() == new_nick) {
      return true;
    }
  }
  return false;
}

void CommandManager::update_nickname(Client &client, const std::string &new_nick) { client.set_nickname(new_nick); }

void CommandManager::broadcast_message(const std::string &msg, int sender_fd) {
  for (ClientMap::iterator it = server._clients.begin(); it != server._clients.end(); ++it) {
    int client_fd = it->first;
    if (client_fd != sender_fd) {
      server._message_queues[client_fd].push(msg);
    }
  }
}

void CommandManager::broadcast_nickname_change(int sender_fd, const std::string &old_nick,
                                               const std::string &new_nick) {
  const std::string msg = ":" + old_nick + " NICK " + new_nick + "\r\n";
  broadcast_message(msg, sender_fd);
}

void CommandManager::user(Commands &command) {
  std::vector<std::string> params = command.get_parameters();
  if (params.size() < 3) {
    // Handle error: not enough parameters
    return;
  }
  Client &client = command.get_sender();
  update_user_info(client, params[0], params[2]);
  std::cout << "Client username is: " << client.get_username() << std::endl;
}

void CommandManager::send_welcome_messages(Client &client) {
  if (!client.is_authenticated()) return;

  server.send_message(client.get_fd(), RPL_WELCOME(client.get_nickname(), client.get_username()));
  server.send_message(client.get_fd(), RPL_YOURHOST(client.get_nickname()));
  server.send_message(client.get_fd(), RPL_CREATED(client.get_nickname()));
  server.send_message(client.get_fd(), RPL_MYINFO(client.get_nickname(), "", ""));
}

void CommandManager::update_user_info(Client &client, const std::string &username, const std::string &realname) {
  client.set_username(username);
  client.set_hostname(client.get_fd());  // Pass the sender_fd as the socket descriptor
  client.set_realname(realname);
}
