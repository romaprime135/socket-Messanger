#pragma once
#include <string>

struct Peer {
    std::string username;
    std::string ip;
    int port;

    Peer ( ) = default;
    Peer ( const std::string &un, const std::string &ip, int port )
        : username ( un ), ip ( ip ), port ( port ) { }
};