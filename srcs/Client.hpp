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
	bool _pass_is_correct;
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
    const std::string &get_nickname() const;
    const std::string &get_username() const;
    const std::string &get_realname() const;
    const std::string &get_hostname() const;
    void set_hostname(int client_socket);
    bool is_authenticated() const;
    void set_authentication();
    std::string get_buffer() const;
    void set_nickname(const std::string &nickname);
    void set_username(const std::string &username);
    void set_realname(const std::string &realname);
};
