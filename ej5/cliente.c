//***********************
// Nombre del Script:            ./cliente
// Trabajo Practico Nro:         3
// Ejercicio Nro:                5
// Entrega Nro:                  1
// Integrantes
//
//       Apellidos               Nombre                  Dni
//-------------------------------------------------------------------
//
//       Della Maddalena         Tomas                   39322141
//       Hidalgo                 Nahuel Christian        41427455
//       Feito                   Gustavo                 27027190
//       Pfeiffer                Martin                  39166668
//       Zarzycki                Hernan Alejandro        39244031
//
//***********************


#include <signal.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "game.h"
#include <sys/file.h>
#include <errno.h>

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
    if(argc == 2)
    {
        if(strcmp(argv[1], "h") == 0 || strcmp(argv[1], "help") == 0 || strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "-?") == 0
            || strcmp(argv[1], "--h") == 0 || strcmp(argv[1], "-help") == 0 || strcmp(argv[1], "--help") == 0)
        {
            printf("Esta es el Cliente para el juego HangMan.\n\nObjetivo: Es la UI de acceso al juego, el usuario debe descubrir la palabra y solo puede cometer 6 errores."
                            "\n\nLa sintaxis para la ejecucion es: ./cliente [servidor string] [puerto int]\n\nEjemplo: ./cliente 127.0.0.1 8080\n\n");
            exit(1);
        }
        else
        {
            printf("El parametro utilizado no es correcto.\nEjecute el siguiente comando para mas informacion: ./cliente -help\n");
            exit(1);
        }
    }
    
    if(argc != 3){
        printf("El parametro utilizado no es correcto.\nEjecute el siguiente comando para mas informacion: ./cliente -help\n");
        exit(1);
    }

    //https://stackoverflow.com/questions/5339200/how-to-create-a-single-instance-application-in-c-or-c
    int pid_file = open("/tmp/archivolockclientesocket.pid", O_CREAT | O_RDWR, 0666);
    int rc = flock(pid_file, LOCK_EX | LOCK_NB);
    if(rc) {
        if(EWOULDBLOCK == errno)
        {
            printf("Solo se permite una instancia del cliente.\n");
            exit(0);
        }
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

        printf("%s\n", bufferLectura);

    }

    printf("done!\n");

    return 0;
}
