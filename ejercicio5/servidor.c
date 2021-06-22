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
#include "game.h"

//declaraciones
void iniciar_juego(int connfd);

struct sockaddr_in serv_addr;

int senialFinRecibida = 0;

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

    int connfd = accept(se, (struct sockaddr*)NULL, NULL);

    while(senialFinRecibida == 0)
    {
        printf("Conexion del cliente aceptada\n");

        iniciar_juego(connfd);
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

    do{
        bytesRecibidos = read(connfd, buffLectura, sizeof(buffLectura)-1);

        printf("ejecutando\n");

        if(bytesRecibidos > 0)
        {
            buffLectura[bytesRecibidos] = 0;
            printf("Mensaje recibido: %s\n", buffLectura);    
        }

        letra = buffLectura[0];
        found = 0;

        if (strcmp(&letra, "\n") == 0)
        {
            strcpy(sendMessage, "Ingrese un caracter valido.\n");
            send(connfd, sendMessage, strlen(sendMessage), 0);

            continue;
        }
    
        if(!isalpha(letra))
        {
            strcpy(sendMessage, "Ingrese un caracter valido.\n");
            send(connfd, sendMessage, strlen(sendMessage), 0);

            continue;
        }
        else {
            letra = tolower(letra);
            if(strchr(letrasUsadas, letra) != NULL)
            {
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

        send(connfd, sendMessage, strlen(sendMessage), 0);

		win = strchr(guessed, '_');

        seguirJugando = (errores < INTENTOS && win != NULL && terminarPartida == 0);


    }while(seguirJugando);

    strcpy(sendMessage, "\0");

    if(win == NULL) {
        snprintf(sendMessage, sizeof(sendMessage), "\n%s\n\tFelicitaciones! Has Ganado. La palabra era: %s\n\nPresiona ENTER para jugar de nuevo o Ctrl + C para finalizar.", printWord(guessed, len), word);
	} else {
        snprintf(sendMessage, sizeof(sendMessage), "\n%s\n\tHas Perdido!. La palabra era: %s\n\nPresiona ENTER para jugar de nuevo o Ctrl + C para finalizar.", printBody(errores, body), word);
	}

    send(connfd, sendMessage, strlen(sendMessage), 0);

	free(body);
	free(guessed);
    free(letrasUsadas);

    read(connfd, NULL, 0);

	// return EXIT_SUCCESS;
    return;

}
