#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#define CLIENTES 3

struct argsServer{
    char* nombreCliente;
    int novoCalcetinFD;
    int* DFsClientes;
};

struct argsReceptor{
    int soquetFD;
    socklen_t clienteLenght;
    int *DFsClientes;
    struct info_Fio_Server *fios;
};

struct info_Fio_Server{
    pthread_t id;
    struct argsServer *args;
};

struct info_Fio_Receptor{
    pthread_t id;
    struct argsReceptor *receptor1;
};

int numeroDeClientes = 0;

int cerrar = 0;

void *lanzarFioServer(void *ptr){
    struct argsServer *args = ptr;
    char buffer[255];
    char nombre[32];
    char send[288];
    bzero(send, 288);
    long datosLeidosEscritos;
    int PrimeraIteracion = 0;
    int comparacion = -1;

    while(1){
        if (cerrar == -1){
            printf("%s was disconnected\n", nombre);
            write(args->novoCalcetinFD , "La conexión está siendo cortada por el host...\n", 49);
            break;
        }

        bzero(buffer, 255);
        datosLeidosEscritos = read(args->novoCalcetinFD, buffer, 255);

        if (cerrar == -1){
            printf("%s was disconnected\n", nombre);
            write(args->novoCalcetinFD , "La conexión está siendo cortada por el host...\n", 49);
            break;
        }

        if(datosLeidosEscritos < 0){
            perror("Se te jodio el mensaje tipo");
            exit(1);
        }

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
                    datosLeidosEscritos = write(args->DFsClientes[iterator] , send, 288);
                    if(args->DFsClientes[iterator] == args->novoCalcetinFD){
                        close(args->DFsClientes[iterator]);
                        args->DFsClientes[iterator] = -1;
                    }
                }else{
                    if(args->DFsClientes[iterator] != args->novoCalcetinFD)
                        datosLeidosEscritos = write(args->DFsClientes[iterator] , send, 288);
                }
                if(datosLeidosEscritos < 0){
                    perror("Se te jodio el mensaje tipo");
                    exit(1);
                }
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

void* receptor(void* ptr){

    struct argsReceptor *argsReceptor = ptr;
    int novoSoquetFD;
    struct sockaddr_in client_Address;


    while (1) {

        novoSoquetFD = accept(argsReceptor->soquetFD, (struct sockaddr *) &client_Address, &argsReceptor->clienteLenght);

        if (novoSoquetFD < 0){
            perror("Rechazao subnormal");
            exit(1);
        }

        if(numeroDeClientes < CLIENTES){
            argsReceptor->DFsClientes[numeroDeClientes] = novoSoquetFD;
            argsReceptor->fios[numeroDeClientes].args = malloc(sizeof(struct argsServer));
            argsReceptor->fios[numeroDeClientes].args->DFsClientes = argsReceptor->DFsClientes;
            argsReceptor->fios[numeroDeClientes].args->novoCalcetinFD = novoSoquetFD;
            if (0 != pthread_create(&argsReceptor->fios[numeroDeClientes].id, NULL, lanzarFioServer, argsReceptor->fios[numeroDeClientes].args)) {
                free(argsReceptor->fios[numeroDeClientes].args);
                argsReceptor->DFsClientes[numeroDeClientes] = -1;
                printf("Could not create thread #%d", numeroDeClientes);
                exit(1);
            }
            numeroDeClientes++;
        }else{
            printf("Rechazando Cliente: Numero de clientes maximos alcanzado\n");
            char* mensajeRechazo = "Conexion Rechazada: Numero de clientes maximos alcanzado\n";
            write(novoSoquetFD, mensajeRechazo, strlen(mensajeRechazo));
            close(novoSoquetFD);
        }
    }
}

void lanzarServer(int nomeroPorto) {

    int DFsClientes[CLIENTES];
    for (int i = 0; i < CLIENTES; ++i)
        DFsClientes[i] = -1;

    if (nomeroPorto <= 1024) {
        fprintf(stderr, "Formato: ServerThreadTCP <Numero de puerto>\n");
        exit(1);
    }

    struct info_Fio_Server *fios;
    fios = malloc(sizeof(struct info_Fio_Server) * CLIENTES);

    struct info_Fio_Receptor *fioReceptor;
    fioReceptor = malloc(sizeof(struct info_Fio_Receptor));

    if (fios == NULL || fioReceptor == NULL) {
        fprintf(stderr, "Not enough memory\n");
        exit(1);
    }

    struct sockaddr_in server_Address;
    int soquetFD;
    socklen_t clienteLenght;

    soquetFD = socket(AF_INET, SOCK_STREAM, 0);
    if (soquetFD < 0){
        free(fios);
        free(fioReceptor);
        perror("Error abriendo el soquet(putada)");
        exit(1);
    }

    bzero((char *) &server_Address, sizeof(server_Address));

    server_Address.sin_family = AF_INET;
    server_Address.sin_addr.s_addr = INADDR_ANY;
    server_Address.sin_port = htons(nomeroPorto);

    if (bind(soquetFD, (struct sockaddr *) &server_Address, sizeof(server_Address)) < 0){
        free(fios);
        free(fioReceptor);
        close(soquetFD);
        perror("Fallo el bindeo(putada)");
        exit(1);
    }

    listen(soquetFD, CLIENTES);
    clienteLenght = sizeof(struct sockaddr_in);

    printf("Server Initialized!\n");

    fioReceptor->receptor1 = malloc(sizeof (struct argsReceptor));
    fioReceptor->receptor1->soquetFD = soquetFD;
    fioReceptor->receptor1->clienteLenght = clienteLenght;
    fioReceptor->receptor1->DFsClientes = DFsClientes;
    fioReceptor->receptor1->fios = fios;

    if(0 != pthread_create(&fioReceptor->id, NULL, receptor, fioReceptor->receptor1)){
        free(fioReceptor->receptor1);
        printf("Could not create thread #%d", numeroDeClientes);
        exit(1);
    }

    printf("Type \"close\" to close it\n");

    char clos3[6];
    fgets(clos3, 6, stdin);
    while (strncmp("close", clos3, 6) != 0){
        fgets(clos3, 6, stdin);
    }

    cerrar = -1;

    pthread_cancel(fioReceptor->id);

    pthread_join(fioReceptor->id, NULL);

    for (int i = 0; i < numeroDeClientes; ++i){
        pthread_join(fios[i].id, NULL);
        free(fios[i].args);
    }

    free(fioReceptor->receptor1);
    free(fios);
    free(fioReceptor);
    close(soquetFD);
}

int main(int argc, char* argv[]){
    if(argc != 2){
        printf("Chequea los parametros negro\n");
        return -1;
    }
    lanzarServer(atoi(argv[1]));
}