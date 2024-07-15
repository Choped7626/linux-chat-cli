#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <strings.h>
#include <linux/limits.h>
#define CLIENTES 3

int main(){
    system("clear");
    char SiNo;
    int puerto;
    char temp[6];
    char hostname[16] = "127.0.0.1";
    char dir[PATH_MAX];

    if(getcwd(dir, sizeof(dir)) == NULL){
        perror("getcwd() error");
        return 1;
    }

    printf("Introduzca puerto a usar y si desea comunicarse con otro ordenador (y/n)\n"
           "[Numero_Puerto] [Y/N]\n");
    scanf("%d %c", &puerto, &SiNo);
    sprintf(temp, "%d", puerto);

    if(SiNo == 'y' || SiNo == 'Y'){
        printf("Introduzca hostname (IPv4)\n");
        scanf("%s" , hostname);
    }

    int numClientes = 1;
    printf("Introduzca numero de clientes a ejecutar rango:(1-%d)\n", CLIENTES);
    scanf("%d", &numClientes);
    while (numClientes < 0 || numClientes > CLIENTES){
        printf("Graciosillo ponlo bien\n");
        scanf("%d", &numClientes);
    }

    printf("Lanzando Servidor y %d clientes\n", CLIENTES);

    char server[256];
    char cliente[256];

    sprintf(server, "gnome-terminal --window --hide-menubar -- bash -c \"./Server %s; exec bash\"", temp);
    sprintf(cliente, "gnome-terminal --window --hide-menubar -- bash -c \"./Client %s %s; exec bash\"", hostname, temp);

    if(SiNo == 'n' || SiNo == 'N')
        system(server);

    for (int i = 0; i < numClientes && i < CLIENTES; ++i) {
        system(cliente);
    }
    //system("make clean");
}
