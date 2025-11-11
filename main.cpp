#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <conio.h>

#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <memory>
#include <fstream>
#include <sstream>
#include <algorithm>

#include "user.h"
#include "message.h"
#include "peer.h"
#include "network.h"

#pragma comment(lib, "ws2_32.lib")

using namespace std;

string inputWithPoll ( UDPSocket &sock, const string &prompt ) {
    cout << prompt;
    string input;

    while ( true ) {
        sock.poll ( );

        if ( _kbhit ( ) ) {
            char ch = _getch ( );
            if ( ch == '\r' ) {
                cout << endl;
                break;
            }
            else if ( ch == '\b' ) {
                if ( !input.empty ( ) ) {
                    input.pop_back ( );
                    cout << "\b \b";
                }
            }
            else if ( ch >= 32 && ch <= 126 ) {
                input += ch;
                cout << ch;
            }
        }

        Sleep ( 50 );
    }

    return input;
}

vector<Peer> loadPeers ( ) {
    vector<Peer> peers;
    ifstream file ( "peers.txt" );
    if ( !file.is_open ( ) ) {
        return peers;
    }

    string line;
    while ( getline ( file, line ) ) {
        if ( line.empty ( ) ) continue;
        istringstream ss ( line );
        string u, ip;
        unsigned short p;
        getline ( ss, u, ';' );
        getline ( ss, ip, ';' );
        ss >> p;
        peers.emplace_back ( u, ip, p );
    }
    return peers;
}

void registerSelfInPeers ( const User *user ) {
    auto peers = loadPeers ( );

    bool found = false;
    for ( const auto &peer : peers ) {
        if ( peer.username == user->getUsername ( ) ) {
            found = true;
            break;
        }
    }

    if ( !found ) {
        ofstream file ( "peers.txt", ios::app );
        file << user->getUsername ( ) << ";" << user->getIP ( ) << ";" << user->getPort ( ) << "\n";
    }
}

void executeSendToAll ( User *user, UDPSocket &sock, const vector<Peer> &peers ) {
    system ( "cls" );
    string text, tag;

    cout << "Введите сообщение: ";
    text = inputWithPoll ( sock, "" );

    cout << "Укажите важность (Low, Medium, High): ";
    tag = inputWithPoll ( sock, "" );

    static const set<string> validTags = { "Low", "Medium", "High" };
    if ( validTags.find ( tag ) == validTags.end ( ) ) {
        cout << "Неверный приоритет!\n";
        cout << "Нажмите любую клавишу для продолжения...";
        _getch ( );
        return;
    }

    Message msg ( "ALL", user->getUsername ( ), text, tag );
    for ( const auto &peer : peers ) {
        if ( peer.username != user->getUsername ( ) ) {
            sock.send ( msg, peer.ip, peer.port );
        }
    }
    cout << "Сообщение отправлено всем!\n";
    cout << "Нажмите любую клавишу для продолжения...";
    _getch ( );
}

void executeSendPrivate ( User *user, UDPSocket &sock, const vector<Peer> &peers ) {
    system ( "cls" );
    string getter, text, tag;

    cout << "Логин получателя: ";
    getter = inputWithPoll ( sock, "" );

    cout << "Сообщение: ";
    text = inputWithPoll ( sock, "" );

    cout << "Приоритет (Low, Medium, High): ";
    tag = inputWithPoll ( sock, "" );

    static const set<string> validTags = { "Low", "Medium", "High" };
    if ( validTags.find ( tag ) == validTags.end ( ) ) {
        cout << "Неверный приоритет!\n";
        cout << "Нажмите любую клавишу для продолжения...";
        _getch ( );
        return;
    }

    auto it = find_if ( peers.begin ( ), peers.end ( ),
        [&]( const Peer &p ) { return p.username == getter; } );
    if ( it == peers.end ( ) ) {
        cout << "Пользователь не найден в сети!\n";
        cout << "Нажмите любую клавишу для продолжения...";
        _getch ( );
        return;
    }

    Message msg ( getter, user->getUsername ( ), text, tag );
    sock.send ( msg, it->ip, it->port );
    cout << "Сообщение отправлено!\n";
    cout << "Нажмите любую клавишу для продолжения...";
    _getch ( );
}

void chatMenu ( User *user, UDPSocket &sock, const vector<Peer> &peers ) {
    sock.setCurrentUser ( user );

    while ( true ) {
        sock.poll ( );

        system ( "cls" );
        cout << "=== Чат (" << user->getUsername ( ) << ") ===" << endl;
        cout << "1. Отправить всем" << endl;
        cout << "2. Личное сообщение" << endl;
        cout << "3. Проверить сообщения" << endl;
        cout << "4. Выход" << endl;
        cout << "Выберите: ";

        string choice = inputWithPoll ( sock, "" );

        if ( choice == "1" ) {
            executeSendToAll ( user, sock, peers );
        }
        else if ( choice == "2" ) {
            executeSendPrivate ( user, sock, peers );
        }
        else if ( choice == "3" ) {
            cout << "Сообщения проверены. Нажмите любую клавишу...";
            _getch ( );
        }
        else if ( choice == "4" ) {
            break;
        }
        else {
            cout << "Неверный выбор!\n";
            cout << "Нажмите любую клавишу для продолжения...";
            _getch ( );
        }
    }
}

unique_ptr<User> loginUser ( ) {
    system ( "cls" );
    string username, password, ip;
    unsigned short port;

    cout << "Логин: ";
    getline ( cin, username );
    cout << "Пароль: ";
    getline ( cin, password );
    cout << "Ваш IP (например, 192.168.1.10): ";
    getline ( cin, ip );
    cout << "Порт (0 = автоматический): ";
    cin >> port;
    cin.ignore ( );

    vector<User> users = loadUsers ( );
    for ( const auto &u : users ) {
        if ( u.getUsername ( ) == username && u.getPassword ( ) == password ) {
            auto user = make_unique<User> ( u.getUsername ( ), u.getPassword ( ), u.getName ( ), ip, port );
            return user;
        }
    }

    cout << "Неверный логин/пароль!\n";
    cout << "Нажмите любую клавишу для продолжения...";
    _getch ( );
    return nullptr;
}

int main ( ) {
    setlocale ( LC_ALL, "RUS" );
    system ( "chcp 1251 > nul" );

    while ( true ) {
        system ( "cls" );
        cout << "=== Мессенджер ===" << endl;
        cout << "1. Регистрация" << endl;
        cout << "2. Вход" << endl;
        cout << "3. Выход" << endl;
        cout << "Выберите: ";

        string choice;
        getline ( cin, choice );

        if ( choice == "1" ) {
            registerUser ( );
        }
        else if ( choice == "2" ) {
            auto user = loginUser ( );
            if ( user ) {
                auto peers = loadPeers ( );
                try {
                    UDPSocket sock ( user->getPort ( ) );
                    user->setPort ( sock.getBoundPort ( ) );
                    registerSelfInPeers ( user.get ( ) );
                    peers = loadPeers ( );
                    chatMenu ( user.get ( ), sock, peers );
                }
                catch ( const exception &e ) {
                    cout << "Ошибка создания сокета: " << e.what ( ) << endl;
                    cout << "Нажмите любую клавишу для продолжения...";
                    _getch ( );
                }
            }
        }
        else if ( choice == "3" ) {
            break;
        }
        else {
            cout << "Неверный выбор!\n";
            cout << "Нажмите любую клавишу для продолжения...";
            _getch ( );
        }
    }

    WSACleanup ( );
    return 0;
}