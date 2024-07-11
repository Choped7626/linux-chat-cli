#include <iostream>
#include <string>

#include "net.h"

int main( ) {
    std::print( "name: " );

    std::string name{ };
    std::getline( std::cin, name );

    if ( !net::client_startup( name ) ) {
        return 1;
    }

    std::println( "connected to server" );

    while ( true ) {
        std::string msg{ };
        std::getline( std::cin, msg );

        int res = send( net::client_socket, msg.c_str( ), ( int ) msg.length( ), 0 );
        if ( res == SOCKET_ERROR ) {
            break;
        }
    }

    closesocket( net::client_socket );
    WSACleanup( );

    return 0;
}
