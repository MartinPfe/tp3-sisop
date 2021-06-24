//***********************
// Nombre del Script:            ./cliente
// Trabajo Practico Nro:         3
// Ejercicio Nro:                4
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
#include <sys/mman.h>
#include <fcntl.h> 
#include <semaphore.h>
#include "game.h"

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
    // printf("Le aviso al server y me voy\n");
    strcpy(bufferSincro, "/fin");
    escribirEnMemoriaCompartida(dir_M_SERVER);
    sem_post(semaforoCliente);
    printf("Gracias por jugar!\n");
    exit(0);
}

int main(int argc, char *argv[])
{
    if(argc > 1){
        if(strcmp(argv[1], "h") == 0 || strcmp(argv[1], "help") == 0 || strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "-?") == 0
            || strcmp(argv[1], "--h") == 0 || strcmp(argv[1], "-help") == 0 || strcmp(argv[1], "--help") == 0)
        {
                    printf("Esta es el Cliente para el juego HangMan.\n\nObjetivo: Es la UI de acceso al juego, el usuario debe descubrir la palabra y solo puede cometer 6 errores."
                            "\n\nLa sintaxis para la ejecucion es:\t./cliente\n");
            exit(1);
        }
        else
        {
            printf("El parametro utilizado no es correcto.\nEjecute el siguiente comando para mas informacion: ./cliente -help\n");
            exit(1);
        }
    }

    signal(SIGINT, sigintHandler);
    semaforoServer = obtenerSemaforo("server");
    semaforoCliente = obtenerSemaforo("cliente");

	dir_M_SERVER = (char *)abrirMemoriaCompartida("M_SERVER", sizeof(bufferSincro));
 	char* p;
   
    printf("Bienvenido al Hangman!\nSolo puedes cometer 6(seis) errores.\n\nComencemos!\n\n\n\n");
    sem_wait(semaforoServer);

    // strcpy(bufferSincro, (char*)dir_M_SERVER);

    // printf("%s \n", bufferSincro);
    //fflush(stdin);

    while (1)
    {
		sem_wait(semaforoServer);
        memset(bufferSincro, 0, sizeof bufferSincro);
        strcpy(bufferSincro, (char*)dir_M_SERVER);
     
		printf("%s \n", bufferSincro);
        
        if (strstr(bufferSincro, "El servidor a finalizado la conexion.") > 0)
        {
            exit(0);
        }

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