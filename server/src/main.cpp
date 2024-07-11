#include <iostream>
#include <thread>
#include <vector>

#include "net.h"

struct client_t {
    std::string name{ };
    SOCKET socket{ };
};

static void handle_client( const client_t& client ) {
    std::println( "{} joined", client.name );

    while ( true ) {
        std::array< char, 1024 > recv_buf{ };
        if ( int result = recv( client.socket, recv_buf.data( ), recv_buf.size( ), 0 ); result > 0 ) {
            std::println( "{}> {}", client.name, std::string_view( recv_buf.data( ), result ) );
        } else {
            std::println( "{} left", client.name );
            break;
        }
    }

    closesocket( client.socket );
}

int main( ) {
    if ( !net::server_startup( ) ) {
        return 1;
    }

    std::println( "server running on port 33333" );

    while ( true ) {
        SOCKET client_socket = net::accept_client( );
        if ( client_socket == INVALID_SOCKET ) {
            break;
        }

        // std::println( "got client. socket {}", client_socket );

        std::array< char, 1024 > recv_buf{ };
        if ( int result = recv( client_socket, recv_buf.data( ), recv_buf.size( ), 0 ); result > 0 ) {
            // std::println( "got welcome msg: {}", std::string_view( recv_buf.data( ), result ) );

            // handle welcome message
            // @note: -1 result for some reason
            //
            if ( std::string_view( recv_buf.data( ), result - 1 ).compare( "welcome" ) ) {
                std::println( "invalid welcome msg from {}", client_socket );
                closesocket( client_socket );
                continue;
            }

            const char* accepted = "accepted";
            send( client_socket, accepted, sizeof( accepted ), 0 );

            // std::println( "accepted msg sent" );

            result = recv( client_socket, recv_buf.data( ), recv_buf.size( ), 0 );

            // std::println( "got name {}", std::string_view( recv_buf.data( ), result ) );

            client_t client{ .name = std::string( recv_buf.data( ), result ), .socket = client_socket };

            std::jthread client_handle( handle_client, client );
            client_handle.detach( );
        } else if ( result == 0 ) {
            std::println( "connection closing..." );
        } else {
            std::println( "recv failed: {}", WSAGetLastError( ) );
            break;
        }
    }

    return 0;
}