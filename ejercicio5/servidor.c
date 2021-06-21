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


    
    do{
        if(errores == 0) 
            strcpy(errorsMessage, "None\n");
		for (int i = 0; i < errores; ++i)
		{
            if(i>0)
                strcat(errorsMessage, "-");
            else
                strcpy(errorsMessage, "\n");

            cToStr[0] = falseWord[i];    
            cToStr[1] = '\0';    
			strcat(errorsMessage, cToStr);
		}
        // sendMessage = "\n\n"+ printBody +"\n\n\tFalse Letters: " + errorsMessage + "\n\n " + printWord + " \tGive me a alphabet in lower case: "


        // snprintf(sendMessage, sizeof(sendMessage),"\n\n%s\n\n\tFalse Letters: %s\n\n %s \tGive me a alphabet in lower case: \n",
        //             printBody(errores, body),errorsMessage,printWord(guessed, len));

         snprintf(sendMessage, sizeof(sendMessage),"%s\n\n\tFalse Letters: %s\n\n\t %s",printBody(errores, body),errorsMessage, printWord(guessed, len));            

        send(connfd, sendMessage, strlen(sendMessage), 0);

        bytesRecibidos = read(connfd, buffLectura, sizeof(buffLectura)-1);
        if(bytesRecibidos > 0)
        {
            buffLectura[bytesRecibidos] = 0;
            printf("Mensaje recibido: %s\n", buffLectura);    
            
            tryLetter(word, len, guessed, falseWord, &errores, buffLectura[0]);

            // printf("Hello message sent\n");
        }
		win = strchr(guessed, '_');
    }while(errores < INTENTOS && win != NULL);

    strcpy(sendMessage, "\0");

    if(win == NULL) {
        printf("palabra: %s\n", word);
        snprintf(sendMessage, sizeof(sendMessage), "\n%s\n\tFelicitaciones! Has Ganado. La palabra era: %s\n\n", printWord(guessed, len), word);
	} else {
        printf("palabra: %s\n", word);
        snprintf(sendMessage, sizeof(sendMessage), "\n%s\n\tHas Perdido!. La palabra era: %s\n\n", printBody(errores, body), word);
	}

    send(connfd, sendMessage, strlen(sendMessage), 0);

	free(body);
	free(guessed);
	// return EXIT_SUCCESS;
    return;

}