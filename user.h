#pragma once
#include <string>
#include <vector>

class User {
    std::string username;
    std::string password;
    std::string name;
    std::string IP;
    int Port;

public:
    User ( const std::string &UN, const std::string &PW, const std::string &Name, const std::string &IP, int Port );
    std::string getUsername ( ) const;
    std::string getPassword ( ) const;
    std::string getName ( ) const;
    std::string getIP ( ) const;
    int getPort ( ) const;
    void setPort ( int port );
};

std::vector<User> loadUsers ( );
void registerUser ( );