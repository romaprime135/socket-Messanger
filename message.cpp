#include "message.h"
#include <sstream>

Message::Message ( const std::string &g, const std::string &s, const std::string &t, const std::string &tg )
    : Getter ( g ), Sender ( s ), Text ( t ), Tag ( tg ) { }

std::string Message::getData ( ) const {
    return Sender + ";" + Getter + ";" + Text + ";" + Tag;
}

Message Message::getMessage ( const std::string &data ) {
    std::istringstream ss ( data );
    std::string parts [4];
    for ( int i = 0; i < 4 && std::getline ( ss, parts [i], ';' ); ++i );
    if ( !parts [0].empty ( ) && !parts [1].empty ( ) && !parts [2].empty ( ) && !parts [3].empty ( ) )
        return Message ( parts [1], parts [0], parts [2], parts [3] );
    return Message ( );
}