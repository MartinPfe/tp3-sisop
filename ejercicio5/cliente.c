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
int se;

void sigintHandler(int sig_num)
{
    // printf("Le aviso al server y me voy\n");
    strcpy(bufferEscritura, "/fin");
    write(se, bufferEscritura, strlen(bufferEscritura));
    printf("Gracias por jugar!\n");
    exit(0);
}

int main(int argc, char *argv[])
{
   if(argc != 2){
        if(strcmp(argv[1], "h") == 0 || strcmp(argv[1], "help") == 0 || strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "-?") == 0
            || strcmp(argv[1], "--h") == 0 || strcmp(argv[1], "-help") == 0 || strcmp(argv[1], "--help") == 0)
        {
                    printf("Esta es el Cliente para el juego HangMan.\n\nObjetivo: Es la UI de acceso al juego, el usuario debe descubrir la palabra y solo puede cometer 6 errores."
                            "\n\nLa sintaxis para la ejecucion es: ./cliente [servidor string] [puerto int]\n\nEjemplo: ./cliente 127.0.0.1 8080\n\n");
            exit(1);
        }
        // else
        // {
        //     printf("El parametro utilizado no es correcto.\nEjecute el siguiente comando para mas informacion: ./cliente -help\n");
        //     exit(1);
        // }
    }

    signal(SIGINT, sigintHandler);

    char *servidor = argv[1];
    int puerto_server = strtol(argv[2],NULL,10);;

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
    
	if(inet_pton(AF_INET, servidor, &serv_addr.sin_addr)<=0)
	{
		printf("\nDireccion invalida\n");
		return -1;
	}

    if(connect(se, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("Error en connect");
        exit(EXIT_FAILURE);
    }

    printf("Bienvenido al Hangman!\nSolo puedes cometer 6(seis) errores.\n\nComencemos!\n\n\n\n");

    read(se, bufferLectura, 1024);
    printf("%s\n", bufferLectura);

    while(1)
    {
     
        char ingresado[1024];
        char* p;

        do {
            fflush(stdin);
            fgets(ingresado, sizeof(ingresado), stdin);

            if ((p = strchr(ingresado, '\n')) != NULL)
                *p = '\0';

        } while(strlen(ingresado) == 0);

        write(se, ingresado, strlen(ingresado));
        memset(bufferLectura, 0, sizeof bufferLectura);
        read(se, bufferLectura, sizeof(bufferLectura));


        if (strstr(bufferLectura, "/fin") > 0)
        {
            printf("El servidor finalizo.\n");
            exit(0);
        }

        printf("**********Recibido************\n");
        printf("%s\n", bufferLectura);
        printf("**********FIN Recibido************\n");


    }

    printf("done!\n");

    return 0;
}
