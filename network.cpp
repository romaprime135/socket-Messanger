#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <iostream>
#include "network.h"
#include "user.h"

#pragma comment(lib, "ws2_32.lib")

static bool winsockInitialized = false;

UDPSocket::UDPSocket ( unsigned short port ) {
    if ( !winsockInitialized ) {
        WSADATA wsaData;
        WSAStartup ( MAKEWORD ( 2, 2 ), &wsaData );
        winsockInitialized = true;
    }

    sock = socket ( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
    if ( sock == INVALID_SOCKET ) throw std::runtime_error ( "Socket creation failed" );

    u_long mode = 1;
    ioctlsocket ( sock, FIONBIO, &mode );

    sockaddr_in addr { };
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons ( port );

    if ( bind ( sock, (sockaddr *) &addr, sizeof ( addr ) ) == SOCKET_ERROR ) {
        closesocket ( sock );
        throw std::runtime_error ( "Bind failed" );
    }

    int len = sizeof ( addr );
    getsockname ( sock, (sockaddr *) &addr, &len );
    localPort = ntohs ( addr.sin_port );
}

UDPSocket::~UDPSocket ( ) {
    closesocket ( sock );
}

unsigned short UDPSocket::getBoundPort ( ) const {
    return localPort;
}

void UDPSocket::setCurrentUser ( User *user ) {
    currentUser = user;
}

void UDPSocket::send ( const Message &msg, const std::string &ip, unsigned short port ) {
    std::string data = msg.getData ( );
    sockaddr_in dest { };
    dest.sin_family = AF_INET;
    dest.sin_port = htons ( port );
    inet_pton ( AF_INET, ip.c_str ( ), &dest.sin_addr );
    sendto ( sock, data.c_str ( ), data.size ( ), 0, (sockaddr *) &dest, sizeof ( dest ) );
}

void UDPSocket::poll ( ) {
    if ( !currentUser ) return;

    char buffer [4096];
    sockaddr_in from;
    int fromLen = sizeof ( from );

    while ( true ) {
        int bytes = recvfrom ( sock, buffer, sizeof ( buffer ) - 1, 0, (sockaddr *) &from, &fromLen );
        if ( bytes <= 0 ) break;

        buffer [bytes] = '\0';
        Message msg = Message::getMessage ( std::string ( buffer ) );

        if ( ( msg.Getter == currentUser->getUsername ( ) || msg.Getter == "ALL" ) &&
            msg.Sender != currentUser->getUsername ( ) ) {

            std::cout << "\n[NEW MESSAGE] From: " << msg.Sender
                << " | Priority: " << msg.Tag
                << "\nText: " << msg.Text << "\n\n";
        }
    }
}

bool UDPSocket::hasData ( int timeoutMs ) {
    fd_set fds;
    FD_ZERO ( &fds );
    FD_SET ( sock, &fds );

    timeval tv = { 0, timeoutMs * 1000 };
    return select ( 0, &fds, NULL, NULL, &tv ) > 0;
}