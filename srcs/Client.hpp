#pragma once

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <string>

class Server;

class Client {
  private:
    int _fd;
    std::string _nick;
    std::string _user;
    std::string _name;
    std::string _hostname;
	std::string _pass;
    bool _authenticated;
    std::string _buffer;
    Server* _server;
    // std::vector _joinedChannels;

  public:
    Client(int fd,  Server* server);
    ~Client();

    void clean_buffer();
    bool buffer_has_linebreak();
    bool read_into_buffer();

    // getters:
    int get_fd() const;
	const std::string &get_pass() const;
    const std::string &get_nickname() const;
    const std::string &get_username() const;
    const std::string &get_realname() const;
    const std::string &get_hostname() const;
    const std::string &get_buffer() const;
    Server* getServer() const;

	// setters:
    void set_authentication(bool status);
    void set_hostname(int client_socket);
    void set_nickname(const std::string &nickname);
    void set_username(const std::string &username);
    void set_realname(const std::string &realname);

	// checkers:
	bool password_matched(Server &server) const;
    bool is_authenticated() const;
};
