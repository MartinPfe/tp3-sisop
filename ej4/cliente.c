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

sem_t *obtenerSemaforo(const char *nombre);
void borrarSemaforo(const char * nombre, sem_t *semaforo);

void * abrirMemoriaCompartida(const char *nombre, size_t size);
void borrarMemoriaCompartida(const char *nombre, size_t size, void *direccion);
void escribirEnMemoriaCompartida(char * dir_M_SERVER);

sem_t *semaforoServer;
sem_t *semaforoCliente;
char bufferSincro[1024]; //Buffer de escritura
char lecturaTeclado[1024]; //Buffer de escritura
char *dir_M_SERVER;
void sigintHandler(int sig_num)
{
    printf("Le aviso al server y me voy\n");
    strcpy(bufferSincro, "fin");
    escribirEnMemoriaCompartida(dir_M_SERVER);
    sem_post(semaforoCliente);
    printf("Gracias por jugar!\n");
    exit(0);
}

int main()
{
    signal(SIGINT, sigintHandler);
    semaforoServer = obtenerSemaforo("server");
    semaforoCliente = obtenerSemaforo("cliente");

	dir_M_SERVER = (char *)abrirMemoriaCompartida("M_SERVER", sizeof(bufferSincro));
 	char* p;

    printf("Bienvenido al Hangman. Comencemos!\n\n\n\n");
   
    sem_wait(semaforoServer);

    // strcpy(bufferSincro, (char*)dir_M_SERVER);

    // printf("%s \n", bufferSincro);
    //fflush(stdin);

    while (1)
    {
		sem_wait(semaforoServer);
        strcpy(bufferSincro, (char*)dir_M_SERVER);
		printf("%s \n", bufferSincro);

        do {
            fflush(stdin);
            fgets(lecturaTeclado, sizeof(lecturaTeclado), stdin);

            if ((p = strchr(lecturaTeclado, '\n')) != NULL)
                *p = '\0';

          
        } while(strlen(lecturaTeclado) == 0);

        strcpy(bufferSincro, lecturaTeclado);
        escribirEnMemoriaCompartida(dir_M_SERVER);

		sem_post(semaforoCliente);
    }

    printf("done!\n");
}

sem_t * obtenerSemaforo(const char *nombre)
{
	return sem_open(nombre, 0); // 0600
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