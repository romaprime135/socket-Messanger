#include "user.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <windows.h>

User::User ( const std::string &UN, const std::string &PW, const std::string &Name, const std::string &IP, int Port )
    : username ( UN ), password ( PW ), name ( Name ), IP ( IP ), Port ( Port ) { }

std::string User::getUsername ( ) const { return username; }
std::string User::getPassword ( ) const { return password; }
std::string User::getName ( ) const { return name; }
std::string User::getIP ( ) const { return IP; }
int User::getPort ( ) const { return Port; }
void User::setPort ( int port ) { Port = port; }

std::vector<User> loadUsers ( ) {
    std::vector<User> users;
    std::ifstream file ( "users.txt" );
    std::string line;
    while ( std::getline ( file, line ) ) {
        if ( line.empty ( ) ) continue;
        std::istringstream ss ( line );
        std::string un, pw, name;
        std::getline ( ss, un, ';' );
        std::getline ( ss, pw, ';' );
        std::getline ( ss, name );
        users.emplace_back ( un, pw, name, "", 0 );
    }
    return users;
}

void registerUser ( ) {
    system ( "cls" );
    std::string un, pw, name;
    std::cout << "Логин: ";
    std::getline ( std::cin, un );
    std::cout << "Пароль: ";
    std::getline ( std::cin, pw );
    std::cout << "Имя: ";
    std::getline ( std::cin, name );

    std::ofstream file ( "users.txt", std::ios::app );
    file << un << ";" << pw << ";" << name << "\n";
    std::cout << "Регистрация успешна!\n";
    std::cout << "Нажмите END...";
    while ( !GetAsyncKeyState ( VK_END ) ) { }
}