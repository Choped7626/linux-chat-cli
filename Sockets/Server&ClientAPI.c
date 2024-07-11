#include "commons.h"

void gilipollas(const char *numError){
    perror(numError);
    exit(1);
}

struct args{
    int id_Cliente;
    int novoCalcetinFD;
};

struct info_Fio{
    pthread_t id;
    struct args *args;
};

void *lanzarFioServer(void *ptr){
    struct args *args = ptr;
    long cnt;
    char buffer[255];
    char nombre[32];
    int i = 0;

    while(1){
        bzero(buffer, 255);
        cnt = read(args->novoCalcetinFD, buffer, 255);
        if(cnt < 0)
            gilipollas("Se te jodio el mensaje tipo");
        if(i == 0){
            for (int j = 0; j < 32; ++j) {
                if (buffer[j] == '\n')
                    break;
                nombre[j] = buffer[j];
            }
            i++;
            printf("%s connected to the chat\n", nombre);
        }else{
            i = strncmp("Exit" , buffer , 4);///cambiar este texto e numero de caracteres para cambiar mensaje de salir
            if (i == 0){
                printf("%s left the chat\n", nombre);
                break;
            }
            printf("%s: %s", nombre, buffer);
        }
    }
    close(args->novoCalcetinFD);
    return NULL;
}

void lanzarServer(int nomeroPorto) {

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
    struct timeval timeout;

    timeout.tv_sec = TIMEOUT;
    timeout.tv_usec = 0;

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

    int n = 0;
    while (1) {

        novoSoquetFD = accept(soquetFD, (struct sockaddr *) &client_Address, &clienteLenght);

        if (novoSoquetFD < 0)
            gilipollas("Rechazao subnormal");

        fios[n].args = malloc(sizeof(struct args));
        fios[n].args->id_Cliente = n;
        fios[n].args->novoCalcetinFD = novoSoquetFD;

        if (0 != pthread_create(&fios[n].id, NULL, lanzarFioServer, fios[n].args)) {
            printf("Could not create thread #%d", n);
            exit(1);
        }
        n++;

        if (n == CLIENTES) {
            printf("Limite de clientes alcanzado\n");
            break;
        }
    }

    for (int i = 0; i < n; ++i){
        pthread_join(fios[i].id, NULL);
        free(fios[i].args);
    }

    free(fios);
    close(novoSoquetFD);
    close(soquetFD);
}

void lanzarCliente(char* hostName, int nomeroPorto){
    struct sockaddr_in server_Address;
    int soquetFD, cnt;
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

    while(1){
        bzero(buffer, 255);
        fgets(buffer, 255, stdin);
        int i = strncmp("Exit", buffer, 4);

        cnt = write(soquetFD, buffer, strlen(buffer));
        if(cnt < 0)
            gilipollas("Fallo en escritura");

        if(i == 0){
            cnt = write(soquetFD, buffer, strlen(buffer));
            if(cnt < 0)
                gilipollas("Fallo en escritura");
            break;
        }
    }
    close(soquetFD);
}

