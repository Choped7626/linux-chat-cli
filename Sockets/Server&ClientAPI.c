#include "commons.h"

void gilipollas(const char *numError){
    perror(numError);
    exit(1);
}

struct args{
    char* nombreCliente;
    int novoCalcetinFD;
    int* DFsClientes;
};

struct info_Fio{
    pthread_t id;
    struct args *args;
};

int numeroDeClientes = 0;

void *lanzarFioServer(void *ptr){
    struct args *args = ptr;
    char buffer[255];
    char nombre[32];
    char send[287];
    long datosLeidosEscritos;
    int PrimeraIteracion = 0;
    int comparacion = -1;

    while(1){
        bzero(buffer, 255);
        datosLeidosEscritos = read(args->novoCalcetinFD, buffer, 255);
        if(datosLeidosEscritos < 0)
            gilipollas("Se te jodio el mensaje tipo");
        if(PrimeraIteracion == 0){
            char* tok = strtok(buffer, "\n");
            strcpy(nombre, tok);
            PrimeraIteracion++;
            sprintf(send , "%s connected to the chat\n", nombre);
            printf("%s", send);
        }else {
            sprintf(send, "%s: %s", nombre, buffer);
            printf("%s", send);
            comparacion = strncmp("Exit", buffer, 4);
        }
            int iterator = 0;
            while (iterator < CLIENTES){
                if (args->DFsClientes[iterator] != -1){
                    if(comparacion == 0){
                        sprintf(send, "%s left the chat\n" , nombre);
                        datosLeidosEscritos = write(args->DFsClientes[iterator] , send, 287);
                        if(args->DFsClientes[iterator] == args->novoCalcetinFD){
                            close(args->DFsClientes[iterator]);
                            args->DFsClientes[iterator] = -1;
                        }
                    }else{
                        if(args->DFsClientes[iterator] != args->novoCalcetinFD)
                            datosLeidosEscritos = write(args->DFsClientes[iterator] , send, 287);
                    }
                    if(datosLeidosEscritos < 0)
                        gilipollas("Se te jodio el mensaje tipo");
                }
                iterator++;
            }
            if (comparacion == 0){
                printf("%s left the chat\n", nombre);
                numeroDeClientes--;
                break;
            }

    }
    close(args->novoCalcetinFD);
    return NULL;
}

void lanzarServer(int nomeroPorto) {

    int DFsClientes[CLIENTES];
    for (int i = 0; i < CLIENTES; ++i)
        DFsClientes[i] = -1;

    if (nomeroPorto <= 1024) {
        fprintf(stderr, "Formato: ServerThreadTCP <Numero de puerto>\n");
        exit(1);
    }

    struct info_Fio *fios;
    fios = malloc(sizeof(struct info_Fio) * CLIENTES);

    if (fios == NULL) {
        fprintf(stderr, "Not enough memory\n");
        exit(1);
    }

    struct sockaddr_in server_Address, client_Address;
    int soquetFD, novoSoquetFD;
    socklen_t clienteLenght;

    soquetFD = socket(AF_INET, SOCK_STREAM, 0);
    if (soquetFD < 0)
        gilipollas("Error abriendo el soquet(putada)");

    bzero((char *) &server_Address, sizeof(server_Address));

    server_Address.sin_family = AF_INET;
    server_Address.sin_addr.s_addr = INADDR_ANY;
    server_Address.sin_port = htons(nomeroPorto);

    if (bind(soquetFD, (struct sockaddr *) &server_Address, sizeof(server_Address)) < 0)
        gilipollas("Fallo el bindeo(putada)");

    listen(soquetFD, CLIENTES);
    clienteLenght = sizeof(client_Address);

    while (1) {

        novoSoquetFD = accept(soquetFD, (struct sockaddr *) &client_Address, &clienteLenght);

        DFsClientes[numeroDeClientes] = novoSoquetFD;

        if (novoSoquetFD < 0)
            gilipollas("Rechazao subnormal");

        fios[numeroDeClientes].args = malloc(sizeof(struct args));
        //fios[numeroDeClientes].args->id_Cliente = numeroDeClientes;
        fios[numeroDeClientes].args->DFsClientes = DFsClientes;
        fios[numeroDeClientes].args->novoCalcetinFD = novoSoquetFD;

        if (0 != pthread_create(&fios[numeroDeClientes].id, NULL, lanzarFioServer, fios[numeroDeClientes].args)) {
            printf("Could not create thread #%d", numeroDeClientes);
            exit(1);
        }
        numeroDeClientes++;

        if (numeroDeClientes == CLIENTES) {
            printf("Limite de clientes alcanzado\n");
            break;
        }
    }

    for (int i = 0; i < numeroDeClientes; ++i){
        pthread_join(fios[i].id, NULL);
        free(fios[i].args);
    }

    free(fios);
    close(novoSoquetFD);
    close(soquetFD);
}

void* recibirMensajes(void* ptr){////
    struct args *args = ptr;
    char buffer[287];

    long cnt;
    int i;

    char salir[287];
    sprintf(salir, "%s left the chat\n" , args->nombreCliente);

    while(1){
        bzero(buffer, 287);
        cnt = read(args->novoCalcetinFD, buffer, 287);

        if(cnt < 0)
            gilipollas("Se te jodio el mensaje tipo");

        printf("%s", buffer);
        i = strcmp(salir , buffer);
        if (i == 0)
            break;
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
        fprintf(stderr, "Pon bien el host gilipollas\n");
        exit(1);
    }

    if(nomeroPorto <= 1024){
        fprintf(stderr, "Formato: ClientTCP <Nombre de Host/IPv4> <Numero de puerto>\n");
        exit(1);
    }

    soquetFD = socket(AF_INET, SOCK_STREAM, 0);
    if (soquetFD < 0)
        gilipollas("no abre el calcetin");

    bzero((char *) &server_Address, sizeof (server_Address));
    bcopy((char *) server->h_addr , (char *) &server_Address.sin_addr.s_addr, server->h_length);
    server_Address.sin_family = AF_INET;
    server_Address.sin_port = htons(nomeroPorto);

    if(connect(soquetFD, (struct sockaddr *) &server_Address, sizeof (server_Address)) < 0)
        gilipollas("Conexion refusada");

    printf("Introduzca su nombre de usuario:(max 32 char)\n");
    scanf("%s", nombre);
    cnt = write(soquetFD, nombre, strlen(nombre));
    if(cnt < 0)
        gilipollas("Fallo en escritura");

    struct info_Fio *fiosCliente;
    fiosCliente = malloc(sizeof(struct info_Fio));

    fiosCliente->args = malloc(sizeof(struct args));
    fiosCliente->args->nombreCliente = nombre;
    fiosCliente->args->novoCalcetinFD = soquetFD;

    if (0 != pthread_create(&fiosCliente->id, NULL, recibirMensajes, fiosCliente->args)) {
        printf("Could not create thread");
        exit(1);
    }

    while(1){
        bzero(buffer, 255);
        fgets(buffer, 255, stdin);
        int i = strncmp("Exit", buffer, 4);

        if(buffer[0] != '\n')
            cnt = write(soquetFD, buffer, strlen(buffer));
        if(cnt < 0)
            gilipollas("Fallo en escritura");

        if(i == 0){
            //?¿?¿?¿?cnt = write(soquetFD, buffer, strlen(buffer));
            if(cnt < 0)
                gilipollas("Fallo en escritura");
            break;
        }
    }

    pthread_join(fiosCliente->id, NULL);
    free(fiosCliente->args);
    free(fiosCliente);
    close(soquetFD);
}

