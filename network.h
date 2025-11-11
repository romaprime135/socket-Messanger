#pragma once
#include <string>
#include <vector>
#include "message.h"
#include "peer.h"

class User;

class UDPSocket {
    int sock;
    unsigned short localPort;
    User *currentUser = nullptr;

public:
    UDPSocket ( unsigned short port );
    ~UDPSocket ( );
    void send ( const Message &msg, const std::string &ip, unsigned short port );
    unsigned short getBoundPort ( ) const;
    void setCurrentUser ( User *user );
    void poll ( );
    bool hasData ( int timeoutMs );
};