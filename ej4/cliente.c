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

void * crearMemoriaCompartida(const char *nombre, size_t size);
void borrarMemoriaCompartida(const char *nombre, size_t size, void *direccion);
void escribirEnMemoriaCompartida(char * dir_M_SERVER);

sem_t *semaforoServer;
sem_t *semaforoCliente;
char bufferSincro[1024]; //Buffer de escritura

int main()
{
    semaforoServer = obtenerSemaforo("server");
    semaforoCliente = obtenerSemaforo("cliente");

	char *dir_M_SERVER = (char *)crearMemoriaCompartida("M_SERVER", sizeof(bufferSincro));
 	char* p;
    while (1)
    {
        printf("dale apreta\n");
    
		sem_wait(semaforoServer);

        strcpy(bufferSincro, (char*)dir_M_SERVER);

		printf("el server dice: %s \n", bufferSincro);

		printf("2\n");
        fflush(stdin);
        printf("3\n");

        fgets(bufferSincro, sizeof(bufferSincro), stdin);
        printf("4\n");

        if ((p = strchr(bufferSincro, '\n')) != NULL)
          *p = '\0';
        printf("5\n");

        escribirEnMemoriaCompartida(dir_M_SERVER);
        printf("6\n");

		sem_post(semaforoCliente);
        printf("7\n");
    }

    printf("done!\n");
}

sem_t * obtenerSemaforo(const char *nombre)
{
	return sem_open(nombre, 0); // 0600
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