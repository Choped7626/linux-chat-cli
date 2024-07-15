#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#include <sys/ioctl.h>
#define CLIENTES 3

struct argsCliente{
    char* nombreCliente;
    int novoCalcetinFD;
    int* DFsClientes;
};

struct info_Fio_Cliente{
    pthread_t id;
    struct argsCliente *args;
};

void gilipollas2(const char *numError){
    perror(numError);
    exit(1);
}

char* posPointer(){
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    char* buff = (char *) malloc(w.ws_col + 1);
    printf("\033[%d;0H", w.ws_row);
    fgets(buff, w.ws_col + 1, stdin);
    buff[strlen(buff) - 1] = '\0';
    return buff;
}

int cerrar = 0;

void* recibirMensajes(void* ptr){
    struct argsCliente *args = ptr;
    char buffer[287];

    long cnt;
    int i;
    int pechar;

    char salir[287];
    sprintf(salir, "%s left the chat\n" , args->nombreCliente);

    while(1){
        bzero(buffer, 287);
        cnt = read(args->novoCalcetinFD, buffer, 287);

        if(cnt < 0)
            gilipollas2("Se te jodio el mensaje tipo");

        printf("%s", buffer);

        pechar = strncmp("La conexión está siendo cortada por el host...\n", buffer, 49);
        i = strcmp(salir , buffer);
        if (i == 0 || pechar == 0){
            cerrar = 1;
            break;
        }
    }
    close(args->novoCalcetinFD);
    return NULL;
}

void lanzarCliente(char* hostName, int nomeroPorto){
    struct sockaddr_in server_Address;
    int soquetFD; long cnt;
    struct hostent *server;
    char buffer[255];
    char nombre[32];

    server = gethostbyname(hostName);

    if(server == NULL){
        fprintf(stderr, "Pon bien el host gilipollas2\n");
        exit(1);
    }

    if(nomeroPorto <= 1024){
        fprintf(stderr, "Formato: ClientTCP <Nombre de Host/IPv4> <Numero de puerto>\n");
        exit(1);
    }

    soquetFD = socket(AF_INET, SOCK_STREAM, 0);
    if (soquetFD < 0)
        gilipollas2("no abre el calcetin");

    bzero((char *) &server_Address, sizeof (server_Address));
    bcopy((char *) server->h_addr , (char *) &server_Address.sin_addr.s_addr, server->h_length);
    server_Address.sin_family = AF_INET;
    server_Address.sin_port = htons(nomeroPorto);

    if(connect(soquetFD, (struct sockaddr *) &server_Address, sizeof (server_Address)) < 0)
        gilipollas2("Conexion refusada");

    printf("Introduzca su nombre de usuario:(max 32 char)\n");
    scanf("%s", nombre);
    cnt = write(soquetFD, nombre, strlen(nombre));
    if(cnt < 0)
        gilipollas2("Fallo en escritura");

    struct info_Fio_Cliente *fiosCliente;
    fiosCliente = malloc(sizeof(struct info_Fio_Cliente));

    fiosCliente->args = malloc(sizeof(struct argsCliente));
    fiosCliente->args->nombreCliente = nombre;
    fiosCliente->args->novoCalcetinFD = soquetFD;

    if (0 != pthread_create(&fiosCliente->id, NULL, recibirMensajes, fiosCliente->args)) {
        printf("Could not create thread");
        exit(1);
    }

    while(1){
        if(cerrar != 1){
            bzero(buffer, 255);
            fgets(buffer, 255, stdin);
            int i = strncmp("Exit", buffer, 4);

            if(buffer[0] != '\n')
                cnt = write(soquetFD, buffer, strlen(buffer));
            if(cnt < 0 && cerrar != 1)
                gilipollas2("Fallo en escritura");

            if(i == 0)
                break;
        } else
            break;
    }

    pthread_join(fiosCliente->id, NULL);
    free(fiosCliente->args);
    free(fiosCliente);
    close(soquetFD);
}


int main(int argc, char* argv[]){
    if(argc != 3){
        printf("Chequea los parametros negro\n");
        return -1;
    }
    lanzarCliente(argv[1], atoi(argv[2]));
}