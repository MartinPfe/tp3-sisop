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

struct sockaddr_in serv_addr;

char bufferEscritura[1024];
char bufferLectura[1024];

int main(int argc, char *argv[]){
    if(argc != 2 || strcmp(argv[1], "h") == 0 || strcmp(argv[1], "help") == 0 || strcmp(argv[1], "-h") == 0
        || strcmp(argv[1], "--h") == 0 || strcmp(argv[1], "-help") == 0 || strcmp(argv[1], "--help") == 0)
    {
         printf("Esta es el Cliente para el juego HangMan.\n\nObjetivo: es la UI del usuario para acceder y jugar al juego Hangman, cuenta con 6 Intentos para descubrir la palabra contabilizando diferentes letras ingresadas por el usuario."
                "\n\nLa sintaxis para la ejecucion es:\t./cliente\n");
        exit(1);
    }

    int se;

    char *servidor = "localhost";
    int puerto_server = 8080;
    
    // printf("El server es: %d", (AF_INET));

    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(puerto_server);

    // SOCKET DE ESCUCHA
    //socket(DOMINIO, TIPO, PROTOCOLO);
    if ((se = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("Error en socket");
        exit(EXIT_FAILURE);
    }
    
	if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
	{
		printf("\nDireccion invalida\n");
		return -1;
	}

    if(connect(se, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("Error en connect");
        exit(EXIT_FAILURE);
    }

    printf("Conexion establecida\n");

    read(se, bufferLectura, 1024);
    printf("%s\n", bufferLectura);

    while(1)
    {
        // char* mensaje = "Hello world";

        // read(se, bufferLectura, 1024);
        // printf("%s\n", bufferLectura);

        char ingresado[1024];
        char* p;

        fflush(stdin);
        fgets(ingresado, sizeof(ingresado), stdin);

        if ((p = strchr(ingresado, '\n')) != NULL)
            *p = '\0';

        write(se, ingresado, strlen(ingresado));

        
        read(se, bufferLectura, 1024);
        printf("%s\n", bufferLectura);

    }

    printf("done!\n");

    return 0;
}
