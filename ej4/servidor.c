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
#include <sys/mman.h>
#include <sys/stat.h> // Define constantes de modo
#include <fcntl.h> // Define constantes O_*
#include <semaphore.h>
#include "game.h"

sem_t *crearSemaforo(const char *nombre, int valor);
void borrarSemaforo(const char * nombre, sem_t *semaforo);
sem_t *obtenerSemaforo(const char *nombre);

void * crearMemoriaCompartida(const char *nombre, size_t size);
void borrarMemoriaCompartida(const char *nombre, size_t size, void *direccion);
void * abrirMemoriaCompartida(const char *nombre, size_t size);
void escribirEnMemoriaCompartida(char * dir_M_SERVER);
void iniciar_juego();
void avisarClienteFin();

sem_t *semaforoServer;
sem_t *semaforoCliente;
char bufferSincro[1024]; //Buffer de escritura
int senialFinRecibida = 0;
int isPlaying = 0;
char *dir_M_SERVER;

void sigintHandler(int sig_num)
{

    signal(SIGINT, sigintHandler);
    printf("\n Cannot be terminated using Ctrl+C \n");
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

int main()
{
    signal(SIGINT, sigintHandler);
    signal(SIGUSR1, sigusr1Handler);

    borrarSemaforo("server", semaforoServer);
    borrarSemaforo("cliente", semaforoCliente);

    semaforoServer = crearSemaforo("server", 1);
    semaforoCliente = crearSemaforo("cliente", 0);

    char *respuesta = "";

    dir_M_SERVER = (char *)crearMemoriaCompartida("M_SERVER", sizeof(bufferSincro));

	//Si habia algo lo borro
	if(dir_M_SERVER)
	{
        printf("Borramos la memoria compartida\n");
		borrarMemoriaCompartida("M_SERVER", sizeof(int), dir_M_SERVER);
		dir_M_SERVER = (char *)crearMemoriaCompartida("M_SERVER", sizeof(bufferSincro));
	}

    printf("Empezando a escuchar\n");
    while (senialFinRecibida == 0)
    {
        iniciar_juego(dir_M_SERVER);
    }

    borrarSemaforo("server", semaforoServer);
    borrarSemaforo("cliente", semaforoCliente);

    printf("Fin del juego. Adios!!\n");
}

void avisarClienteFin()
{
    strcpy(bufferSincro, "STOP");
    printf("Avisandole al otro gil");
    escribirEnMemoriaCompartida(dir_M_SERVER);
    sem_post(semaforoServer);
}

void iniciar_juego(char * dir_M_SERVER)
{
	char *body = malloc(INTENTOS+1);

    //obtener palabra
    char word[256];
    char *pword = getWord();
    strcpy(word, pword);
    printf("La palabra a adivinar es: %s\n", word);

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
    char *letrasUsadas = malloc(28);
	memset(letrasUsadas, ' ', 28);

    char cToStr[2];
    char letra;

    char *respuesta = "";
    int seguirJugando = 1;
    int intentos = 0;
    int found = 0;
    int terminarPartida = 0;

    snprintf(bufferSincro, sizeof(bufferSincro),"%s\n\n\tLetras usadas: %s\n\n\t %s",printBody(errores, body),errorsMessage, printWord(guessed, len));            
    escribirEnMemoriaCompartida(dir_M_SERVER);
    sem_post(semaforoServer);
    do{
        sem_wait(semaforoCliente);
        isPlaying = 1;

        strcpy(bufferSincro, (char*)dir_M_SERVER);

        printf("Mensaje recibido: %s\n", bufferSincro);    
            
        bufferSincro[3] = '\0';
            
        if (strcmp(bufferSincro, "fin") == 0)
        {
            printf("ingresado FINNN");
            terminarPartida = 1;
            break;
        }

        letra = bufferSincro[0];
        found = 0;

        if (strcmp(&letra, "\n") == 0)
        {
            strcpy(bufferSincro, "Ingrese un caracter valido.");
            escribirEnMemoriaCompartida(dir_M_SERVER);
            sem_post(semaforoServer);
            continue;
        }
    
        if(!isalpha(letra))
        {
            strcpy(bufferSincro, "Ingrese un caracter valido.");
            escribirEnMemoriaCompartida(dir_M_SERVER);
            sem_post(semaforoServer);
            continue;
        }
        else {
            letra = tolower(letra);
            if(strchr(letrasUsadas, letra) != NULL)
            {
                strcpy(bufferSincro, "La letra que ingresaste esta repetida.");
                escribirEnMemoriaCompartida(dir_M_SERVER);
                sem_post(semaforoServer);
                continue;
            }
        }

        tryLetter(word, len, guessed, falseWord, &errores, letra);

        printf("Lo que tiene que adivinar es: %s\n", word);

        strcpy(bufferSincro, (char*)dir_M_SERVER);

        letrasUsadas[intentos] = letra;
        intentos++;

        printf("Letras usadas: %s\n", letrasUsadas);

        snprintf(bufferSincro, sizeof(bufferSincro),"%s\n\n\tLetras usadas: %s\n\n\t %s",printBody(errores, body),letrasUsadas, printWord(guessed, len));            
    
        escribirEnMemoriaCompartida(dir_M_SERVER);

        printf("Esperando que manden lo siguiente\n");    

		win = strchr(guessed, '_');

        seguirJugando = (errores < INTENTOS && win != NULL && terminarPartida == 0);

        if(seguirJugando)
        {
            sem_post(semaforoServer);
        }
       
    }
    while(seguirJugando);
    isPlaying = 0;

    printf("YA ME FUI\n");

    if (terminarPartida == 1)
    {
        printf("Partida terminada por pedido del usuario.\n");
    }
    else
    {
        if(win == NULL) {
            snprintf(bufferSincro, sizeof(bufferSincro), "\n%s\n\tFelicitaciones! Has Ganado. La palabra era: %s\n\nPresiona ENTER para jugar de nuevo o Ctrl + C para finalizar.", printWord(guessed, len), word);
        } else {
            snprintf(bufferSincro, sizeof(bufferSincro), "\n%s\n\tHas Perdido!. La palabra era: %s\n\nPresiona ENTER para jugar de nuevo o Ctrl + C para finalizar.", printBody(errores, body), word);
        }
        escribirEnMemoriaCompartida(dir_M_SERVER);
    }

	free(body);
	free(guessed);
    free(letrasUsadas);

    sem_post(semaforoServer);
    if (terminarPartida == 0)
    {
        sem_wait(semaforoCliente);

        strcpy(bufferSincro, (char*)dir_M_SERVER);

        printf("Mensaje recibido: %s\n", bufferSincro);    
            
        bufferSincro[3] = '\0';
            
        if (strcmp(bufferSincro, "fin") == 0)
        {
            sem_post(semaforoServer);
        }
    }

	// return EXIT_SUCCESS;
    return;

}

sem_t * crearSemaforo(const char *nombre, int valor)
{
	return sem_open(nombre, O_CREAT, S_IRUSR | S_IWUSR, valor); // 0600
}

void * crearMemoriaCompartida(const char *nombre, size_t size)
{
	int fd = shm_open(nombre, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR); // O_EXCL
	ftruncate(fd, size); // Setea el tamaño de la memoria

	void *dir = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	return dir;
}

void * abrirMemoriaCompartida(const char *nombre, size_t size)
{
    int fd = shm_open(nombre, O_RDWR, 0);
    ftruncate(fd, size); // Setea el tamaño de la memoria

    void *dir = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    return dir;
}

void borrarMemoriaCompartida(const char *nombre, size_t size, void *direccion)
{
	munmap(direccion, size);
	shm_unlink(nombre);
}

void escribirEnMemoriaCompartida(char * dir_M_SERVER)
{
	if(*bufferSincro)
	{
    	strcpy(dir_M_SERVER, bufferSincro);    
    	//printf("Envio -> %s\n", sendBuff);
    }
}

void borrarSemaforo(const char * nombre, sem_t *semaforo)
{
	sem_close(semaforo);
	sem_unlink(nombre);
}

sem_t * obtenerSemaforo(const char *nombre)
{
	return sem_open(nombre, 0); // 0600
}
