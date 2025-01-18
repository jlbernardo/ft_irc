#pragma once
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <cstdlib>
#include <iostream>
#include <map>
#include "Client.hpp"

#include <string>

class Channel {
	
	private:

		std::string         _name;          // Channel name (starts with # or &)
		std::string         _topic;         // Channel topic
		std::string         _key;           // Channel password (if mode +k is set)
		std::map<int, Client*> _members;    // Channel members (fd -> Client*)
		std::map<int, Client*> _operators;  // Channel operators (fd -> Client*)
		
		// Channel modes
		bool _inviteOnly;      // mode +i
		bool _topicRestricted; // mode +t
		bool _hasKey;          // mode +k
		size_t _userLimit;     // mode +l

	public:	

		Channel(const std::string& name, Client* creator);
		~Channel();

		// Basic operations
		bool addMember(Client* client);
		bool removeMember(Client* client);
		void broadcastMessage(const std::string& message, Client* sender = NULL);
		
		// Channel operator operations
		void setOperator(Client* client, bool status);
		bool isOperator(Client* client) const;
		
		// Mode operations
		void setMode(char mode, bool status);
		bool checkMode(char mode) const;
		
		// Getters
		const std::string& getName() const;
		const std::string& getTopic() const;
		const std::map<int, Client*>& getMembers() const;
		
		// Required by subject
		bool kickMember(Client* operator_client, Client* target, const std::string& reason);
		bool inviteMember(Client* operator_client, Client* target);
		bool setTopic(Client* client, const std::string& new_topic);
};#include "Client.hpp"
#include "ft_irc.h"
#include <iostream>

Client::Client(int fd) : _fd(fd), _nick("unknown"), _user("unknown"), _name("unknown"), _authenticated(false), _buffer("") {
  set_hostname(fd);
}

Client::~Client() {}

void Client::set_hostname(int client_socket) {
  struct sockaddr_in addr;
  socklen_t len = sizeof(addr);
  if (getsockname(client_socket, (struct sockaddr*)&addr, &len) == -1) {
    _hostname = "unknown";
    println("getsockname has failed, thus the name is: ");
    printlnnl(_hostname);
    return;
  }
  _hostname = inet_ntoa(addr.sin_addr);
  println("The hostname will be ");
  printlnnl(_hostname);
}

bool Client::read_into_buffer() {
  static char temp[1024];  // to automatically zero-initialize
  ssize_t bytes_read = recv(_fd, temp, sizeof(temp) - 1, 0); // SEGFAULT HERE
  if (bytes_read <= 0) {
    return false;
  }
  _buffer.append(temp, bytes_read);
  println(_buffer);
  return true;
}

int Client::get_fd() const { 
  return _fd; 
}

const std::string& Client::get_nickname() const {
    return _nick; 
}

const std::string& Client::get_username() const {
    return _user;
}

const std::string& Client::get_realname() const {
    return _name;
}

const std::string& Client::get_hostname() const { 
  return _hostname; 
}

bool Client::is_authenticated() const {
   return _authenticated;
}

std::string Client::get_buffer() const {
  return _buffer;
}

void Client::set_nickname(const std::string& nickname) {
  _nick = nickname;
}

void Client::set_username(const std::string& username) {
  _user = username;
}

void Client::set_realname(const std::string& realname) {
  _name = realname;
}

void Client::set_authentication(bool status) {
  _authenticated = status;
}

void Client::clean_buffer() { 
  _buffer.clear();
}

bool Client::buffer_has_linebreak() {
   return _buffer.find("\n") != std::string::npos;
}
#pragma once
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <string>

class Client {
 private:
  int _fd;
  std::string _nick;
  std::string _user;
  std::string _name;
  std::string _hostname;
  bool _authenticated;
  std::string _buffer;
  // std::vector _joinedChannels; 

 public:
  Client(int fd);
  ~Client();

  void clean_buffer();
  bool buffer_has_linebreak();
  bool read_into_buffer();

  // accessors:
  int get_fd() const;
  const std::string& get_nickname() const;
  const std::string& get_username() const;
  const std::string& get_realname() const;
  const std::string& get_hostname() const;
  void set_hostname(int client_socket);
  bool is_authenticated() const;
  void set_authentication(bool status);
  std::string get_buffer() const;
  void set_nickname(const std::string& nickname);
  void set_username(const std::string& username);
  void set_realname(const std::string& realname);
};#include <map>
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
    server.send_message(client.get_fd(), ERR_NEEDMOREPARAMS(command.list.front().command));
    // ERR_ALREADYREGISTERED (462)
  } else if (client.is_authenticated()) {
    server.send_message(client.get_fd(), ERR_ALREADYREGISTERED(command.get_sender().get_username()));
    // ERR_PASSWDMISMATCH (464)
  } else if (param != server.get_pass()) {
    server.send_message(client.get_fd(), ERR_PASSWDMISMATCH());
    server.send_message(client.get_fd(), ERROR(std::string("wrong password")));
    command._fatal_error = true;
  } else {
    client.set_authentication(true);
  }
}

void CommandHandler::nick(Commands &command, const std::string &param) {
  Client &client = command.get_sender();
  if (param.empty()) {
    server.send_message(client.get_fd(), ERR_NONICKNAMEGIVEN());
    return;
  }

  if (is_nickname_in_use(param)) {
    server.send_message(client.get_fd(), ERR_NICKNAMEINUSE(param));
    return;
  }

  std::string old_nick = client.get_nickname();
  update_nickname(client, param);
  broadcast_nickname_change(client, old_nick, param);
  if (!client.is_authenticated()) {
    server.send_message(client.get_fd(), RPL_WELCOME(client.get_nickname(), client.get_username()));
    server.send_message(client.get_fd(), RPL_YOURHOST(client.get_nickname()));
    server.send_message(client.get_fd(), RPL_CREATED(client.get_nickname()));
    server.send_message(client.get_fd(), RPL_MYINFO(client.get_nickname(), "", ""));
    client.set_authentication(true);
  }
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

void CommandHandler::broadcast_nickname_change(Client &client, const std::string &old_nick, const std::string &new_nick) {
  std::string cmd = old_nick + " changed his nickname to " + new_nick + "\r\n";
  // cmd = std::string(":127.0.0.1") + " 001 " + new_nick + " :Welcome to
  // the IRC server! " + new_nick + "!" + "127.0.0.1" + CRLF; for (std::map<int,
  // Client*>::iterator it = clients.begin(); it != clients.end(); ++it) {
  //   if (it->first != client.get_fd()) {
  //     server.send_message(it->first, cmd);
  //   }
  // }
  (void)client;
  // println("Should have printed: " + cmd);
  server.send_message(client.get_fd(), cmd);
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
}

void CommandHandler::update_user_info(Client &client, const std::string &username, const std::string &realname) {
  client.set_username(username);
  client.set_hostname(client.get_fd());  // Pass the sender_fd as the socket descriptor
  client.set_realname(realname);
}

// void CommandHandler::execute_command(const command &command) {}
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
  void broadcast_nickname_change(Client &client, const std::string &old_nick, const std::string &new_nick);
  void update_user_info(Client &client, const std::string &username, const std::string &realname);
  bool is_valid_nickname(const std::string &nickname);
  bool is_valid_username(const std::string &username);
  bool is_nick_in_use(const std::string &new_nick);
};
#include <ctime>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "Client.hpp"
#include "Commands.hpp"
#include "ft_irc.h"

Commands::Commands(Client &sender, const std::string &raw_input) : _sender(sender), _sender_fd(sender.get_fd()), _fatal_error(false) {
  init_command_map();
  init_list_of_commands(raw_input);
}

void Commands::init_command_map() {
  _command_map["PRIVMSG"] = PRIVMSG;
  _command_map["JOIN"] = JOIN;
  _command_map["NICK"] = NICK;
  _command_map["USER"] = USER;
  _command_map["QUIT"] = QUIT;
  _command_map["PASS"] = PASS;
}

void Commands::init_list_of_commands(const std::string &raw_input) {
  std::istringstream iss(raw_input);
  std::vector<std::string> vec;
  vec = separate_multiple_commands(raw_input, CRLF);
  for (std::vector<std::string>::iterator it = vec.begin(); it != vec.end(); it++) {
    std::istringstream cmd(*it);
    Command cmd_obj;
    cmd >> cmd_obj.command;
    getline(cmd, cmd_obj.params);
    cmd_obj.params = trim(cmd_obj.params);
    to_uppercase(cmd_obj.command);
    cmd_obj.type = find_command_type(cmd_obj.command);
    this->list.push_back(cmd_obj);
  }
}

std::vector<std::string> Commands::separate_multiple_commands(const std::string &input, const std::string &delimiter) {
  std::vector<std::string> result;
  size_t start = 0;
  while (start < input.length()) {
    size_t end = input.find_first_of(delimiter, start);
    if (end == std::string::npos) {
      result.push_back(input.substr(start));
      break;
    }
    result.push_back(input.substr(start, end - start));

    start = end + 2;
  }
  return result;
}

void Commands::to_uppercase(std::string &str) {
  for (unsigned int i = 0; i < str.length(); i++) {
    str[i] = std::toupper(str[i]);
  }
}

CommandType Commands::find_command_type(const std::string &cmd) {
  if (_command_map.find(cmd) != _command_map.end()) {
    return _command_map[cmd];
  }
  return UNKNOWN;
}

std::string Commands::format_command() const {
  std::string formatted = ":" + get_sender_info() + " " + _command + " " + _target;
  formatted = add_parameters_to_message(formatted);
  return add_content_to_message(formatted);
}

std::string Commands::get_sender_info() const {
  return _sender.get_nickname() + "!" + _sender.get_username() + "@" + _sender.get_hostname();
}

std::string Commands::add_parameters_to_message(const std::string &msg) const {
  std::string result = msg;
  for (std::vector<std::string>::const_iterator it = _parameters.begin(); it != _parameters.end(); ++it) {
    result += " " + *it;
  }
  return result;
}

std::string Commands::add_content_to_message(const std::string &msg) const {
  if (_content.empty()) {
    return msg;
  }
  return msg + " :" + _content + "\n";
}

std::string Commands::add_timestamp() const {
  char timestamp[32];
  time_t now = time(0);
  strftime(timestamp, sizeof(timestamp), "[%H:%M:%S] ", localtime(&now));
  return std::string(timestamp) + format_command();
}

bool Commands::is_valid() const {
  if (_command.empty() || _target.empty()) {
    return false;
  }
  return validate_command_specific();
}

Client &Commands::get_sender() const { return _sender; }

bool Commands::validate_command_specific() const {
  switch (_command_type) {
    case PRIVMSG:
      return !_content.empty();
    case JOIN:
      return _target[0] == '#';
    case NICK:
      return !_target.empty();
    case USER:
      return _parameters.size() >= 3;
    case QUIT:
      return true;
    default:
      return false;
  }
}

std::string Commands::trim(const std::string &str) {
  if (str.empty()) {
    return str;
  }
  // Find first non-whitespace
  std::string::size_type first = str.find_first_not_of(" \t\n\r\f\v");
  if (first == std::string::npos) {
    return "";  // String is all whitespace
  }
  // Find last non-whitespace
  std::string::size_type last = str.find_last_not_of(" \t\n\r\f\v");
  return str.substr(first, (last - first + 1));
}

std::vector<std::string> Commands::get_parameters() const { return _parameters; }

// COMMENTED OUT BECAUSE WE'RE NOT USING IT AND I DON'T UNDERSTAND IT'S PURPOSE
// void Command::parse_parameters(std::istringstream &iss) {
//   std::string param;
//   while (iss >> param) {
//     if (param[0] == ':') {
//       parse_content_param(iss, param);
//       break;
//     }
//     _parameters.push_back(param);
//   }
// }
//
// void Command::parse_content_param(std::istringstream &iss, const std::string &first_param) {
//   std::string remaining;
//   getline(iss, remaining);
//   _parameters.push_back(first_param.substr(1) + remaining);
//   // this is reconnecting the string with the part that was taken off by the iss when the function parse_parameters
//   were
//   // doing its thing e.g. ":example rest of the command" would got here as iss = rest of the command, and first_param
//   as
//   // :example thus the final string is the concatenation of first_param with the iss without the ":", which is
//   "example
//   // rest of the command"
// }

// void Command::set_content_from_params() {
//   if (!_parameters.empty()) {
//     _content = _parameters.back();
//     _parameters.pop_back();
//   }
// }

// int Tokenizer::count_r_n(const std::string &input, std::string delimiter) {
//   size_t count = 0;
//   size_t pos = 0;
//   if (input.length() == 0) return 0;
//   while ((pos = input.find(delimiter, pos)) != std::string::npos) {
//     count++;
//     pos += 1;
//   }
//   return count;
// }

// CommandType Command::get_command_type() const { return _command_type; }
//
// std::string Command::get_target() const { return _target; }
//
//
// bool Command::is_channel_message() const { return !_target.empty() && _target[0] == '#'; }
//
// bool Command::is_private_message() const { return _command_type == PRIVMSG && !is_channel_message(); }
//
#pragma once
#include <map>
#include <string>
#include <vector>
#include <list>

#include "Client.hpp"

enum CommandType { PRIVMSG, JOIN, NICK, USER, QUIT, PASS, UNKNOWN };

struct Command {
  CommandType type;
  std::string command;
  std::string params;
};

class Commands {
 private:
  std::string _command;
  std::string _target;
  std::string _content;
  Client &_sender;
  const int _sender_fd;
  std::vector<std::string> _parameters;
  std::map<const std::string, CommandType> _command_map;
  CommandType _command_type;

  std::string trim(const std::string &str);
  void init_command_map();
  void parse_parameters(std::istringstream &iss);
  void parse_content_param(std::istringstream &iss, const std::string &first_param);
  void init_list_of_commands(const std::string &raw_message);
  void set_content_from_params();
  std::string add_parameters_to_message(const std::string &msg) const;
  std::string add_content_to_message(const std::string &msg) const;
  bool validate_command_specific() const;
  CommandType find_command_type(const std::string &cmd);
  int count_r_n(const std::string &input, std::string delimiter);
  std::vector<std::string> separate_multiple_commands(const std::string &input, const std::string &delimiter);
  void to_uppercase(std::string &str);

 public:
  Commands(Client &sender, const std::string &raw_message);
  std::string format_command() const;
  CommandType get_command_type() const;
  std::string get_target() const;
  Client &get_sender() const;
  std::vector<std::string> get_parameters() const;
  bool is_valid() const;
  std::string add_timestamp() const;
  std::string get_sender_info() const;
  bool is_channel_message() const;
  bool is_private_message() const;
  std::list<Command> list;
  bool _fatal_error;
};
#pragma once

#include <string>

#define println(x) std::cout << x << std::endl
#define printlnnl(x) std::cout << x

const std::string SERVER = ":big.little.talk.irc";
const std::string CRLF = "\r\n";

inline std::string RPL_WELCOME(const std::string& nick, const std::string& user) {
  return SERVER + " 001 " + user + " :Welcome to the Internet Relay Chat " + nick + "!" + user + "@*" + CRLF;
}

inline std::string RPL_ENDOFWHO(const std::string& channel) {
  return SERVER + " 315 " + channel + " :End of /WHO list." + CRLF;
}

inline std::string RPL_CHANNELMODEIS(const std::string& channel, const std::string& mode,
                                     const std::string& modeParams) {
  return SERVER + " 324 * " + channel + " " + mode + " " + modeParams + CRLF;
}

inline std::string RPL_NOTOPIC(const std::string& nick, const std::string& channel) {
  return SERVER + " 331 " + nick + " " + channel + " :No topic is set" + CRLF;
}

inline std::string RPL_TOPIC(const std::string& nick, const std::string& channel, const std::string& topic) {
  return SERVER + " 332 " + nick + " " + channel + " :" + topic + CRLF;
}

inline std::string RPL_INVITING(const std::string& nick, const std::string& recipient, const std::string& channel) {
  return SERVER + " 341 " + nick + " " + recipient + " :" + channel + CRLF;
}

inline std::string RPL_WHOREPLY(const std::string& channel, const std::string& user, const std::string& nick,
                                const std::string& flags, const std::string& realname) {
  return SERVER + " 352 " + channel + " " + user + " 42sp.org.br ft.irc " + nick + " " + flags + ":0 " + realname +
         CRLF;
}

inline std::string RPL_NAMREPLY(const std::string& nick, const std::string& channel, const std::string& names) {
  return SERVER + " 353 " + nick + " = " + channel + " : " + names + CRLF;
}

inline std::string RPL_ENDOFNAMES(const std::string& nick, const std::string& channel) {
  return SERVER + " 366 " + nick + " " + channel + " : End of names list" + CRLF;
}

// 400s
inline std::string ERR_NOSUCHCHANNEL(const std::string& channel) {
  return SERVER + " 403 * " + channel + " :Invalid channel name!" + CRLF;
}

inline std::string ERR_NOSUCHNICK(const std::string& recipient) {
  return SERVER + " 406 " + recipient + " :No such nick" + CRLF;
}

inline std::string ERR_NORECIPIENT(const std::string& user) {
  return SERVER + " 411 " + user + " :No recipient to message" + CRLF;
}

inline std::string ERR_NOTEXTTOSEND(const std::string& user) {
  return SERVER + " 412 " + user + " :No message to send" + CRLF;
}

inline std::string ERR_NONICKNAMEGIVEN() { return SERVER + " 431 :No nickname given" + CRLF; }

inline std::string ERR_ERRONEUSNICKNAME(const std::string& nick) {
  return SERVER + " 432 * " + nick + " :Nickname is invalid" + CRLF;
}

inline std::string ERR_NICKNAMEINUSE(const std::string& nick) {
  return SERVER + " 433 * " + nick + " :Nickname is already in use" + CRLF;
}

inline std::string ERR_USERNOTINCHANNEL(const std::string& operator_, const std::string& client,
                                        const std::string& channel) {
  return SERVER + " 441 " + operator_ + " " + client + " " + channel + " :They aren't on that channel" + CRLF;
}

inline std::string ERR_NOTONCHANNEL(const std::string& channel) {
  return SERVER + " 442 * " + channel + " :Client not on channel!" + CRLF;
}

inline std::string ERR_USERONCHANNEL(const std::string& nick, const std::string& channel) {
  return SERVER + " 443 * " + nick + " " + channel + " :Client on channel!" + CRLF;
}

inline std::string ERR_NEEDMOREPARAMS(const std::string& command) {
  return SERVER + " 461 * " + command + " " + ":Not enough parameters" + CRLF;
}

inline std::string ERR_ALREADYREGISTERED(const std::string& user) {
  return SERVER + " 462 " + user + " :User already registered" + CRLF;
}

inline std::string ERR_PASSWDMISMATCH() {
  return SERVER + " 464 * :Password was either not given or was incorrect" + CRLF;
}

inline std::string ERR_CHANNELISFULL(const std::string& channel) {
  return SERVER + " 471 * " + channel + " :Channel is full!" + CRLF;
}

inline std::string ERR_INVITEONLYCHAN(const std::string& channel) {
  return SERVER + " 473 * " + channel + " :Channel is invite-only!" + CRLF;
}

inline std::string ERR_BADCHANNELKEY(const std::string& user, const std::string& channel) {
  return SERVER + " 475 " + user + " " + channel + " :Password for channel was either not given or incorrect" + CRLF;
}

inline std::string ERR_CHANOPRIVSNEEDED(const std::string& user, const std::string& channel) {
  return SERVER + " 482 " + user + " " + channel + " :You're not a channel operator!" + CRLF;
}

inline std::string ERROR(const std::string message) { return ":ERROR " + message + CRLF; }

inline std::string RPL_JOIN(const std::string& user, const std::string& channel) {
  return SERVER + user + " JOIN " + channel + CRLF;
}

inline std::string PRIVMSG_BROADCAST(const std::string& nick, const std::string& user, const std::string& channel,
                                     const std::string& topic) {
  return SERVER + nick + "!~" + user + "@ft.irc TOPIC " + channel + " " + topic + CRLF;
}

inline std::string RPL_PRIVMSG(const std::string& user, const std::string& dest, const std::string& message) {
  return SERVER + user + " PRIVMSG " + dest + " :" + message + CRLF;
}

inline std::string RPL_PARTMSG(const std::string& nick, const std::string& user, const std::string& dest,
                               const std::string& message) {
  return SERVER + nick + "!~" + user + "@* PART " + dest + " :" + message + CRLF;
}

inline std::string RPL_PARTNOMSG(const std::string& nick, const std::string& user, const std::string& dest) {
  return SERVER + nick + "!" + user + "@* PART " + dest + CRLF;
}

inline std::string RPL_KICKREASON(const std::string& op_nick, const std::string& op_user, const std::string& channel,
                                  const std::string& client, const std::string& reason) {
  return SERVER + op_nick + "!" + op_user + "@ft.irc KICK " + channel + " " + client + " :" + reason + CRLF;
}

inline std::string RPL_KICKNOREASON(const std::string& op_nick, const std::string& op_user, const std::string& channel,
                                    const std::string& client) {
  return SERVER + op_nick + "!" + op_user + "@ft.irc KICK " + channel + " " + client + CRLF;
}

inline std::string RPL_KICKFEEDBACK(const std::string& op_nick, const std::string& op_user, const std::string& channel,
                                    const std::string& client) {
  return SERVER + op_nick + "!" + op_user + "@ft.irc KICK " + channel + " " + client + CRLF;
}

inline std::string RPL_MODEBASE(const std::string& nick, const std::string& user, const std::string& channel) {
  return SERVER + nick + "!" + user + "@ft.irc MODE " + channel + " ";
}

inline std::string RPL_INVITEMSG(const std::string& nick, const std::string& user, const std::string& recipient,
                                 const std::string& channel) {
  return SERVER + nick + "!~" + user + "@ft.irc INVITE " + recipient + " :" + channel + CRLF;
}

inline std::string RPL_YOURHOST(const std::string& user) {
  return SERVER + " 002 " + user + " :Your host is " + SERVER + ", running version 1.0" + CRLF;
}

#include <ctime>

inline std::string getCurrentDate() {
  std::time_t now = std::time(NULL);
  char buf[100];
  std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
  return std::string(buf);
}

inline std::string RPL_CREATED(const std::string& user) {
  return SERVER + " 003 " + user + " :This server was created " + getCurrentDate() + CRLF;
}

inline std::string RPL_MYINFO(const std::string& user, const std::string& userModes, const std::string& channelModes) {
  return SERVER + " 004 " + user + " " + SERVER + " " + "1.0 " + userModes + " " + channelModes + CRLF;
}

inline std::string ERR_NICKCOLLISION(const std::string& nick, const std::string& user, const std::string& host) {
  return SERVER + " 436 " + SERVER + " " + nick + " :Nickname collision KILL from " + user + "@" + host + CRLF;
}
#include <cstdlib>
#include <stdexcept>
#include "InputValidator.hpp"
#include "ft_irc.h"

namespace InputValidator {
  void validatePort(int argc, const char* port_str) {
	if (argc != 3) {
	  throw std::invalid_argument("Invalid number of arguments");
	}
	for (const char* c = port_str; *c != '\0'; ++c) {
	  if (!std::isdigit(*c)) {
		throw std::invalid_argument("Port must be a number");
	  }
	}
	int port = std::atoi(port_str);
	if (port <= MIN_PORT || port > MAX_PORT) {
	  throw std::invalid_argument("Port must be between 1024 and 65535");
	}
  }

  void validatePassword(const std::string& password) {
	if (password.length() != PASSWORD_LENGTH) {
	  throw std::invalid_argument("\033[31mPassword must be exactly 4 characters\033[0m");
	}
  }
}
#pragma once

#include <string>

namespace InputValidator {
  const int MIN_PORT = 1024;
  const int MAX_PORT = 65535;
  const size_t PASSWORD_LENGTH = 4;
  const std::string pass = "89aX";

  void validatePort(int argc, const char *port_str);
  void validatePassword(const std::string &password);
}
#include <cstdlib>
#include <iostream>

#include "InputValidator.hpp"
#include "Server.hpp"

static void print_usage(const char* program_name);

int main(int argc, char* argv[]) {
  try {
    InputValidator::validatePort(argc, argv[1]);
    InputValidator::validatePassword(argv[2]);

    int port = std::atoi(argv[1]);
    const std::string pass = argv[2];
    Server irc_server(port, pass);
    irc_server.start();

  } catch (const std::invalid_argument& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    print_usage(argv[0]);
    return 1;
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }
}

static void print_usage(const char* program_name) {
  std::cerr << "Usage: " << program_name << " <port> <password>" << std::endl;
  std::cerr << "Port must be between 1024 and 65535" << std::endl;
  std::cerr << "Password must be 4 characters" << std::endl;
}
#include <arpa/inet.h>
#include <csignal>
#include <cstring>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string.h>
#include <unistd.h>

#include "Server.hpp"
#include "SocketsManager.hpp"

template <typename T> std::string to_string(T value) {
  std::ostringstream oss;
  oss << value;
  return oss.str();
}

volatile sig_atomic_t Server::terminate = 0;

Server::Server(int port, const std::string& pass) : _port(port), _pass(pass), _max_fd(0) {
  register_signals();
  initialize_socket();
  setup_server();
}

void Server::register_signals() {
  int signals[6] = {SIGINT, SIGTERM, SIGQUIT, SIGHUP, SIGPIPE, SIGABRT};
  for (size_t i = 0; i < 6; ++i) {
    signal(signals[i], signal_handler);
  }
}

void Server::signal_handler(int signum) { terminate = signum; }

void Server::initialize_socket() {
  _fd = socket(AF_INET, SOCK_STREAM, 0);
  if (_fd < 0) {
    throw std::runtime_error("Failed to create socket\n");
  }
  memset(&_server_addr, 0, sizeof(_server_addr));
  _server_addr.sin_family = AF_INET;
  _server_addr.sin_addr.s_addr = INADDR_ANY;
  _server_addr.sin_port = htons(_port);
}

void Server::setup_server() {
  if (bind(_fd, (struct sockaddr *)&_server_addr, sizeof(_server_addr)) < 0) {
    close(_fd);
    throw std::runtime_error("Failed to bind socket\n");
  } else if (listen(_fd, SOMAXCONN) < 0) {
    close(_fd);
    throw std::runtime_error("Failed to listen on socket\n");
  }
  FD_ZERO(&_master_set);
  FD_SET(_fd, &_master_set);
  _max_fd = _fd;
}

void Server::start() {
  SocketsManager manager(*this);
  while (true) {
    manager.add_new_sockets_from_masterset_to_read_write();
    manager.io_multiplexing();
    for (int fd = 0; fd <= _max_fd; fd++) {
      manager.socket_read(fd);
      manager.socket_write(fd);
    }
  }
}

void Server::add_new_client_to_master_set() {
  struct sockaddr_in client_addr;
  socklen_t client_addr_len = sizeof(client_addr);
  int client_fd = accept(_fd, (struct sockaddr *)&client_addr, &client_addr_len);
  if (client_fd < 0) {
    std::cerr << "Failed to accept connection\n" << std::endl;
    return;
  }
  FD_SET(client_fd, &_master_set);
  if (client_fd > _max_fd)
    _max_fd = client_fd;
  _clients[client_fd] = new Client(client_fd);
#ifdef TEST
  std::stringstream str;
  str << "Connected to server on socket " << client_fd << ": ";
  send(client_fd, str.str().c_str(), str.str().length(), 0);
#endif
}

void Server::remove_client(int client_fd) {
  FD_CLR(client_fd, &_master_set);
  close(client_fd);
  delete _clients[client_fd];
  _clients.erase(client_fd);
}

Server::~Server() {
  std::cout << "\nShutting down server\n";
  for (std::map<int, Client *>::iterator it = _clients.begin();
       it != _clients.end(); ++it) {
    delete it->second;
    close(it->first);
  }
  close(_fd);
}

void Server::send_error(int client_fd, const std::string &error_code, const std::string &error_message) {
  std::string message = ":" + to_string(client_fd) + " " + error_code + " " + error_message + "\r\n";
  send_message(client_fd, message);
}

void Server::send_message(int client_fd, const std::string &message) {
  if (send(client_fd, message.c_str(), message.length(), 0) == -1) {
    std::cerr << "Error sending message to client " << client_fd << std::endl;
  }
}

int Server::get_fd() {
  return _fd;
}

std::string Server::get_pass() {
  return _pass;
}

void Server::set_pass(const std::string& pass){
  _pass = pass;
}

// void Server::stop() {}

// void Server::handlePrivmsg(Client* client, const std::string& command) {} //need implement
// void Server::handlePart(Client* client, const std::string& command){} //need implement
// Channel* Server::createChannel(const std::string& name, Client* creator){} //need implement
// void Server::removeChannel(const std::string& name) {} //need implement
// Channel* Server::findChannel(const std::string& name) {} //need implement
// void Server::handleJoin(Client* client, const std::string& command) {} //need implement
// void Server::handle_client_message(int client_fd) {}
#pragma once

#include <map>
#include <string>
#include "Client.hpp"
#include "Channel.hpp"

class Server {
 private:
  int _fd;
  int _port;
  std::string _pass;
  fd_set _master_set;
  int _max_fd;
  std::map<std::string, Channel*> _channels;
  std::map<int, Client*> _clients;
  struct sockaddr_in _server_addr;
  static void signal_handler(int signum);

  void register_signals();
  void initialize_socket();
  void setup_server();
  void add_new_client_to_master_set();
  void handle_client_message(int client_fd);

 public:
  Server(int port, const std::string& pass);
  ~Server();
  static volatile __sig_atomic_t terminate;
  friend class SocketsManager;
  int get_fd();
  std::string get_pass();
  void set_pass(const std::string& pass);
  void remove_client(int client_fd);
  // Channel* createChannel(const std::string& name, Client* creator); //need implement
  // void removeChannel(const std::string& name); //need implement
  // Channel* findChannel(const std::string& name); //need implement
  // void handleJoin(Client* client, const std::string& command); //need implement
  // void handlePart(Client* client, const std::string& command); //need implement
  // void handlePrivmsg(Client* client, const std::string& command); //need implement

  void start();
  // void stop();
  void send_error(int client_fd, const std::string& error_code, const std::string& error_message);
  void send_message(int client_fd, const std::string& message);
  void error(int fd, const std::string& msg);
};
#include <stdexcept>

#include "Client.hpp"
#include "CommandHandler.hpp"
#include "Commands.hpp"
#include "Server.hpp"
#include "SocketsManager.hpp"

SocketsManager::SocketsManager(Server& serv) : _server(serv) {}

SocketsManager::~SocketsManager() { _message_queues.clear(); }

void SocketsManager::add_new_sockets_from_masterset_to_read_write() {
  FD_ZERO(&_read_set);
  FD_ZERO(&_write_set);
  if (_server._max_fd < FD_SETSIZE) {
    for (int fd = 0; fd <= _server._max_fd; fd++) {
      if (FD_ISSET(fd, &_server._master_set)) {
        FD_SET(fd, &_read_set);
        if (!_message_queues[fd].empty()) {
          FD_SET(fd, &_write_set);
        }
      }
    }
  }
}

void SocketsManager::io_multiplexing() {
  int result = select(_server._max_fd + 1, &_read_set, &_write_set, NULL, NULL);
  if (result < 0) {
    if (_server.terminate)
      throw std::runtime_error("Server received interrupt signal\n");
    else
      throw std::runtime_error("Select failed\n");
  }
}

void SocketsManager::socket_read(int fd) {
  if (FD_ISSET(fd, &_read_set)) {
    if (fd == _server._fd && _server._max_fd < FD_SETSIZE)
      _server.add_new_client_to_master_set();
    else
      load_client_queue(fd);
  }
}

void SocketsManager::load_client_queue(int client_fd) {
  Client& client = *_server._clients[client_fd];
  if (!client.read_into_buffer()) {
    _server.remove_client(client_fd);
    return;
  }
  if (client.buffer_has_linebreak()) {
    Commands cmds(client, client.get_buffer());
    CommandHandler command_handler(_server._clients, _server);
    command_handler.execute(cmds);
    if (cmds._fatal_error)
      _server.remove_client(client_fd);
    else
      client.clean_buffer();
  }
}

void SocketsManager::socket_write(int fd) {
  if (FD_ISSET(fd, &_write_set) && !_message_queues[fd].empty()) {
    while (!_message_queues[fd].empty()) {
      std::string& msg = _message_queues[fd].front();
      if (send(fd, msg.c_str(), msg.length(), 0) > 0)
        _message_queues[fd].pop();
      else
        break;
    }
  }
}
#pragma once
#include <sys/select.h>

#include <map>
#include <queue>
#include <string>

class Server;
class Commands;

class SocketsManager {
public:
  SocketsManager(Server &server);
  ~SocketsManager();
  void add_new_sockets_from_masterset_to_read_write();
  void io_multiplexing();
  void load_client_queue(int fd);
  void socket_read(int fd);
  void socket_write(int fd);

private:
  fd_set _read_set;
  fd_set _write_set;
  std::map<int, std::queue<std::string> > _message_queues; // HOLDS THE FORMATTED MESSAGE OF EVERY CLIENT
  Server &_server;
};
