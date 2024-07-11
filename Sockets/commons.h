#ifndef SOCKETS_COMMONS_H
#define SOCKETS_COMMONS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#define CLIENTES 3
#define TIMEOUT 10

void lanzarServer(int nomeroPorto);

void lanzarCliente(char* hostName, int nomeroPorto);

#endif //SOCKETS_COMMONS_H
