cmake_minimum_required(VERSION 3.28)
project(Sockets C)

set(CMAKE_C_STANDARD 99)

add_executable(Sockets
        main.c
        Server.c
        Client.c
)
