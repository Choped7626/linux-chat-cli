#include <Winsock2.h>
#include <Ws2tcpip.h>
#include <print>
#include <array>

#pragma comment( lib, "Ws2_32.lib" )

namespace net {
    bool initialize_winsock( ) {
        WSADATA wsa_data{ };
        if ( int result = WSAStartup( MAKEWORD( 2, 2 ), &wsa_data );  result != 0 ) {
            std::println( "WSAStartup failed: {}", result );
            return false;
        }

        return true;
    }

    SOCKET server_socket{ };
    SOCKET client_socket{ };

    bool create_socket( SOCKET& aSocket ) {
        aSocket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
        if ( aSocket == INVALID_SOCKET ) {
            std::println( "socket failed: {}", WSAGetLastError( ) );
            WSACleanup( );
            return false;
        }

        return true;
    }

    bool bind_socket( ) {
        sockaddr_in service = { .sin_family = AF_INET, .sin_port = htons( 33333 ) };
        service.sin_addr.s_addr = INADDR_ANY;

        if ( bind( server_socket, ( SOCKADDR* ) &service, sizeof( service ) ) == SOCKET_ERROR ) {
            std::println( "bind failed: {}", WSAGetLastError( ) );
            closesocket( server_socket );
            WSACleanup( );
            return false;
        }

        return true;
    }

    bool listen_on_socket( ) {
        if ( listen( server_socket, SOMAXCONN ) == SOCKET_ERROR ) {
            std::println( "listen failed: {}", WSAGetLastError( ) );
            closesocket( server_socket );
            WSACleanup( );
            return false;
        }

        return true;
    }

    SOCKET accept_client( ) {
        auto temp = accept( server_socket, nullptr, nullptr );
        if ( temp == INVALID_SOCKET ) {
            std::println( "accept failed: {}", WSAGetLastError( ) );
            closesocket( server_socket );
            WSACleanup( );
        }

        return temp;
    }

    bool server_startup( ) {
        if ( !net::initialize_winsock( ) ) {
            return false;
        }

        if ( !net::create_socket( server_socket ) ) {
            return false;
        }

        if ( !net::bind_socket( ) ) {
            return false;
        }

        if ( !net::listen_on_socket( ) ) {
            return false;
        }

        return true;
    }

    bool connect_to_server( std::string_view ip, uint16_t port ) {
        sockaddr_in client_service{ };
        client_service.sin_family = AF_INET;
        inet_pton( AF_INET, ip.data( ), &client_service.sin_addr );
        client_service.sin_port = htons( port );

        if ( connect( client_socket, ( SOCKADDR* ) &client_service, sizeof( client_service ) ) == SOCKET_ERROR ) {
            std::println( "connect failed: {}", WSAGetLastError( ) );
            closesocket( client_socket );
            WSACleanup( );
            return false;
        }

        return true;
    }

    bool client_startup( std::string_view name, std::string_view ip = "127.0.0.1", uint16_t port = 33333 ) {
        if ( !net::initialize_winsock( ) ) {
            return false;
        }

        if ( !net::create_socket( client_socket ) ) {
            return false;
        }

        if ( !net::connect_to_server( ip, port ) ) {
            return false;
        }

        // client -> 'welcome' -> server
        // server -> 'accepted' -> client
        // client -> name -> server
        //
        // @note: error handle send's & recv's and maybe fix cleanup
        //
        const char* welcome = "welcome";
        send( client_socket, welcome, sizeof( welcome ), 0 );
        
        std::array< char, 16 > buf{ };
        if ( int size = recv( client_socket, buf.data( ), buf.size( ), 0 );
             std::string_view( buf.data( ), size ).compare( "accepted" ) ) {
            return false;
        }

        send( client_socket, name.data( ), ( int ) name.length( ), 0 );

        return true;
    }
}