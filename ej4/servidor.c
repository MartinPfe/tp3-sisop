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

sem_t *crearSemaforo(const char *nombre, int valor);
void borrarSemaforo(const char * nombre, sem_t *semaforo);

void * crearMemoriaCompartida(const char *nombre, size_t size);
void borrarMemoriaCompartida(const char *nombre, size_t size, void *direccion);
void escribirEnMemoriaCompartida(char * dir_M_SERVER);

sem_t *semaforoServer;
sem_t *semaforoCliente;
char bufferSincro[1024]; //Buffer de escritura
int senialFinRecibida = 0;

void sigintHandler(int sig_num)
{
    /* Reset handler to catch SIGINT next time.
       Refer http://en.cppreference.com/w/c/program/signal */
    signal(SIGINT, sigintHandler);
    printf("\n Cannot be terminated using Ctrl+C \n");
    fflush(stdout);
}

void sigusr1Handler(int sig_num)
{
    /* Reset handler to catch SIGINT next time.
       Refer http://en.cppreference.com/w/c/program/signal */
    signal(SIGUSR1, sigusr1Handler);
    printf("\n El servidor terminará su ejecución al terminar el juego actual.\n");
    senialFinRecibida = 1;
    fflush(stdout);
}

int main()
{
    signal(SIGINT, sigintHandler);
    signal(SIGUSR1, sigusr1Handler);

    borrarSemaforo("server", semaforoServer);
    borrarSemaforo("cliente", semaforoCliente);

    semaforoServer = crearSemaforo("server", 1);
    semaforoCliente = crearSemaforo("cliente", 0);

	char *dir_M_SERVER = (char *)crearMemoriaCompartida("M_SERVER", sizeof(bufferSincro));

    char *respuesta = "";
    int seguirJugando = 1;

    printf("Empezando a escuchar\n");
    while (seguirJugando)
    {
        
        printf("Esperando a que liberen\n");
        sem_wait(semaforoCliente);
        printf("ya liberaron\n");
        // if (strcmp(bufferSincro, (char *)dir_M_SERVER) == 0)
        //     continue;
            
        strcpy(bufferSincro, (char*)dir_M_SERVER);

        printf("bufferSincro: %s \n", bufferSincro);

        respuesta = "Recibi lo que me dijiste. Mandame de nuevo";

    	strcpy(dir_M_SERVER, respuesta);    

        //escribirEnMemoriaCompartida(dir_M_SERVER);

        sem_post(semaforoServer);

        if(senialFinRecibida == 1)
        {
            seguirJugando = 0;
        }
    }

    borrarSemaforo("server", semaforoServer);
    borrarSemaforo("cliente", semaforoCliente);

    printf("done!\n");
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