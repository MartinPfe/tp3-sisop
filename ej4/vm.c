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

void * abrirMemoriaCompartida(const char *nombre, size_t size);

char bufferSincro[1024]; //Buffer de escritura
char mp[1024]; //Buffer de escritura

int main()
{

	char *dir_M_SERVER = (char *)abrirMemoriaCompartida("M_SERVER", sizeof(bufferSincro));
 	char* p;
    while (1)
    {
        strcpy(bufferSincro, (char*)dir_M_SERVER);

		printf("el server dice: %s \n", bufferSincro);

        fgets(mp, sizeof(mp), stdin);
    }

}

void * abrirMemoriaCompartida(const char *nombre, size_t size)
{
    int fd = shm_open(nombre, O_RDWR, 0);
    ftruncate(fd, size); // Setea el tamaño de la memoria

    void *dir = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    return dir;
}
