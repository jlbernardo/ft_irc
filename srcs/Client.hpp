// client.hpp
#pragma once
#include <string>

class Client {
private:
    int _fd;
    std::string _nick;
    std::string _user;
    std::string _name;
    bool _is_authenticated;

public:
    Client(int fd);
    ~Client();
    
    int get_fd() const;
    const std::string& get_nickname() const;
    const std::string& get_username() const;
    const std::string& get_realname() const;
    bool is_authenticated() const;
    
    void set_nickname(const std::string& nickname);
    void set_username(const std::string& username);
    void set_realname(const std::string& realname);
    void set_authenticated(bool status);
};


