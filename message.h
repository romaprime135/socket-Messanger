#pragma once
#include <string>

struct Message {
    std::string Getter;
    std::string Sender;
    std::string Text;
    std::string Tag;

    Message ( ) = default;
    Message ( const std::string &g, const std::string &s, const std::string &t, const std::string &tg );

    std::string getData ( ) const;
    static Message getMessage ( const std::string &data );
};