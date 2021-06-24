//***********************
// Nombre del Script:            ./servidor
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
#include <netdb.h>
#include "game.h"

//declaraciones
void iniciar_juego(int connfd);

struct sockaddr_in serv_addr;

int senialFinRecibida = 0;
int hayCliente = 0;
int isPlaying = 0;

void sigintHandler(int sig_num)
{
    signal(SIGINT, sigintHandler);
    printf("\n No se puede terminar el servidor Ctrl + C. Use SIGUSR1 (-10).\n");
    fflush(stdout);
}

void sigusr1Handler(int sig_num)
{
    signal(SIGUSR1, sigusr1Handler);
    if (isPlaying == 1){
        printf("\n El servidor terminará su ejecución al terminar el juego actual.\n");
        senialFinRecibida = 1;
        fflush(stdout);
    }
    else{
        printf("\n Gracias por jugar. \n");

        fflush(stdout);
        exit(0);
    }
}

int main(int argc, char *argv[])
{
    if(argc != 2){
        printf("El parametro utilizado no es correcto.\nEjecute el siguiente comando para mas informacion: ./servidor -help\n");
        exit(1);
    }

    if(strcmp(argv[1], "h") == 0 || strcmp(argv[1], "help") == 0 || strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "-?") == 0
        || strcmp(argv[1], "--h") == 0 || strcmp(argv[1], "-help") == 0 || strcmp(argv[1], "--help") == 0)
    {
                printf("Esta es el Servidor para el juego HangMan.\n\nObjetivo: se encarga de manejar toda la logica del juego mediante el mecanismo de IPC: Sockets"
                        "\n\nLa sintaxis para la ejecucion es: ./servidor [puerto int]\n\nEjemplo: ./servidor 8080\n\n");
        exit(1);
    }
    signal(SIGINT, sigintHandler);
    signal(SIGUSR1, sigusr1Handler);

    int puerto_server = strtol(argv[1],NULL,10);;
    int se;

    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(puerto_server);
    
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

    int connfd = 0;

    while(senialFinRecibida == 0)
    {
        if (hayCliente == 1)
        {
            printf("Conexion del cliente aceptada\n");

            iniciar_juego(connfd);
        }
        else {
            printf("Esperando nueva conexion\n");

            connfd = accept(se, (struct sockaddr*)NULL, NULL);
            hayCliente = 1;
        }
    }


    return 0;
}

void iniciar_juego(int connfd)
{
	char *body = malloc(INTENTOS+1);

    //obtener palabra
    char word[256];
    char *pword = getWord();
    strcpy(word, pword);
    printf("La palabra es: %s\n", word);

    // Variables del juego
    int errores = 0;
    int len = strlen(word);
	char *guessed = malloc(len);
	char falseWord[INTENTOS];

	memset(body, ' ', INTENTOS+1);
	memset(guessed, '_', len);
	char guess;
	char* win;
    
    setvbuf(stdin, NULL, _IONBF, 0);

    printf("Juego Inicializado\n");

    char buffLectura[1024];
    memset(buffLectura, '0', sizeof(buffLectura));
    int bytesRecibidos = 0;

    char sendMessage[1024] = "";
    char errorsMessage[10] = "";
    char cToStr[2];

    char *letrasUsadas = malloc(28);
	memset(letrasUsadas, ' ', 28);
    char letra;

    char *respuesta = "";
    int seguirJugando = 1;
    int intentos = 0;
    int found = 0;
    int terminarPartida = 0;

    snprintf(sendMessage, sizeof(sendMessage),"%s\n\n\tLetras usadas: %s\n\n\t %s",printBody(errores, body),letrasUsadas, printWord(guessed, len));            

    send(connfd, sendMessage, strlen(sendMessage), 0);
    isPlaying = 1;
    do{
        bytesRecibidos = read(connfd, buffLectura, sizeof(buffLectura)-1);

        if(bytesRecibidos > 0)
        {
            buffLectura[bytesRecibidos] = 0;
        }

        if (strstr(buffLectura, "/fin") > 0)
        {
            printf("El cliente termino la partida \n");
            terminarPartida = 1;
            seguirJugando = 0;
            hayCliente = 0;
            break;
        }

        if (terminarPartida == 0)
        {
            letra = buffLectura[0];
            found = 0;

            if (strcmp(&letra, "") == 0)
            {
                printf("El cliente ingresó un vacio\n");
                strcpy(sendMessage, "Ingrese un caracter valido.\n");
                send(connfd, sendMessage, strlen(sendMessage), 0);

                continue;
            }
        
            if(!isalpha(letra))
            {
                printf("El cliente ingresó algo que no era caracter\n");
                strcpy(sendMessage, "Ingrese un caracter valido.\n");
                send(connfd, sendMessage, strlen(sendMessage), 0);

                continue;
            }
            else {
                letra = tolower(letra);
                if(strchr(letrasUsadas, letra) != NULL)
                {
                    printf("El cliente ingresó una letra repetida\n");
                    strcpy(sendMessage, "La letra que ingresaste esta repetida.\n");
                    send(connfd, sendMessage, strlen(sendMessage), 0);
                    continue;
                }
            }

            tryLetter(word, len, guessed, falseWord, &errores, buffLectura[0]);

            letrasUsadas[intentos] = letra;
            intentos++;

            printf("Letras usadas: %s\n", letrasUsadas);

            snprintf(sendMessage, sizeof(sendMessage),"%s\n\n\tLetras usadas: %s\n\n\t %s",printBody(errores, body),letrasUsadas, printWord(guessed, len));            

            win = strchr(guessed, '_');

            seguirJugando = (errores < INTENTOS && win != NULL && terminarPartida == 0);

            if (seguirJugando)
            {
                send(connfd, sendMessage, strlen(sendMessage), 0);
            }
        }

    }while(seguirJugando);
    isPlaying = 0;

    strcpy(sendMessage, "\0");

    if(win == NULL) {
        snprintf(sendMessage, sizeof(sendMessage), "\n%s\nFelicitaciones! Has Ganado. La palabra era: %s\n\nIngresa alguna letra para jugar de nuevo o Ctrl + C para finalizar.\n", printBody(errores, body), word);
	} else {
        snprintf(sendMessage, sizeof(sendMessage), "\n%s\nHas Perdido!. La palabra era: %s\n\nIngresa alguna letra para jugar de nuevo o Ctrl + C para finalizar.\n", printBody(errores, body), word);
	}
    send(connfd, sendMessage, strlen(sendMessage), 0);

	free(body);
	free(guessed);
    free(letrasUsadas);


    if (senialFinRecibida==0)
    {
        printf("Esperando a que el cliente presione una tecla\n");    
        bytesRecibidos = read(connfd, buffLectura, sizeof(buffLectura)-1);

        if(bytesRecibidos > 0)
        {
            buffLectura[bytesRecibidos] = 0;
        }

        if (strstr(buffLectura, "/fin") > 0)
        {
            printf("El cliente termino la partida \n");
            hayCliente = 0;
        }
    }
    else {
        printf("Nos mandaron a cerrarnos. Nos vamos\n");    
        sleep(1);
        strcpy(sendMessage, "/fin");
        send(connfd, sendMessage, strlen(sendMessage), 0);
    }

    return;
}
