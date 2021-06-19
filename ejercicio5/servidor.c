// #include <stdio.h>
// #include <stdlib.h>
// #include <sys/socket.h>
// #include <sys/types.h>
// #include <netinet/in.h>
// #include <string.h>

#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

//declaraciones
void iniciar_juego(int connfd);

struct sockaddr_in serv_addr;

int main(){
    int PORT = 8080;
    int se;


    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(PORT);
    
    // SOCKET DE ESCUCHA
    //socket(DOMINIO, TIPO, PROTOCOLO);
    if ((se = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("Error en socket");
        exit(EXIT_FAILURE);
    }
    
    if(bind(se, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("Error en bind");
        exit(EXIT_FAILURE);
    }

    listen(se, 1);

    printf("Esperando conexiones...\n");

    while(1)
    {
        int connfd = accept(se, (struct sockaddr*)NULL, NULL);

        printf("Conexion del cliente aceptada\n");

        iniciar_juego(connfd);
    }


    return 0;
}

void iniciar_juego(int connfd)
{
    printf("Juego Inicializado\n");

    char buffLectura[1024];
    memset(buffLectura, '0', sizeof(buffLectura));
    int bytesRecibidos = 0;

    while(1)
    {
        bytesRecibidos = read(connfd, buffLectura, sizeof(buffLectura)-1);
        if(bytesRecibidos > 0)
        {
            buffLectura[bytesRecibidos] = 0;
            printf("Mensaje recibido: %s\n", buffLectura);    
            
            send(connfd, "hello from server pa", strlen("hello from server pa"), 0);
            printf("Hello message sent\n");
        }
    }
}