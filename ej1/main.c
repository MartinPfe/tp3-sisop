#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>


void crearHijos(int, int, int[]);
void ayuda();

int parentID;

int main(int argc, char *argv[])
{
    int nivelDesc = 0;

    if(argc > 1){
        if(strcmp(argv[1], "h") == 0 || strcmp(argv[1], "help") == 0 || strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "-?") == 0
            || strcmp(argv[1], "--h") == 0 || strcmp(argv[1], "-help") == 0 || strcmp(argv[1], "--help") == 0)
        {
            ayuda();
            exit(1);
        }
        if(atoi(argv[1])<0){
            printf("ERROR, ha ingresado un numero negativo\n");
            exit(4);
        }
         if(!isdigit(*argv[1])){
            printf("Error no ha ingresado un numero como parametro\n");
            exit(5);
        }
    
        int valor=atoi(argv[1]);
    
        if(valor<=0){
            printf("ERROR, ha ingresado un numero negativo o cero\n");
            exit(6);
        } 
    }
    else{
        printf("Ejecute el siguiente comando para obtener mas informacion: ./ej1 -help\n");
        exit(1);
    }

    nivelDesc = atoi(argv[1]);
    printf("El nivel de parametro es %d\n", nivelDesc);
    puts("Para finalizar la ejecucion debe ingresar cualquier tecla.");
    puts("");

    int parents[32];
    parentID = getpid();

    if (nivelDesc > 1)
    {
        crearHijos(1, nivelDesc, parents);
    }
    else if (nivelDesc == 1)
    {
        printf("\nSoy el proceso con PID %d y pertenezco a la generación %d.\n", getpid(), 1);
    }

    getchar();
    kill(-parentID,SIGINT);

    return 0;
}

void ayuda(){
    printf("Ingrese un numero N para crear N generaciones de procesos\n");
    printf("Ejemplo de ejecucion: ./ej1 5\n");
}

void crearHijos(int nivelActual, int nivelDesc, int parents[])
{
    pid_t hijo1;
    pid_t hijo2;

    int nivelAux = nivelActual-1;

    printf("Proceso %d", getpid());
    while(nivelAux > 0)
    {
        if(nivelAux == (nivelActual-1))
            printf(": ");
        else
            printf(", ");

        printf("Pid %d", parents[nivelAux]);
        nivelAux--;
    }
    puts("");

    // printf("Soy el proceso con PID %d y pertenezco a la generación %d.\n", getpid(), nivelActual);

    if (nivelActual < nivelDesc)
    {
        parents[nivelActual] = getpid();

        hijo1 = fork();
        if (hijo1 > 0)
        {

            hijo2 = fork();
            if (hijo2 > 0)
            {
                wait(NULL);
            }
            else if (hijo2 == 0)
            {
                // parents[nivelActual] = getpid();
                crearHijos(nivelActual + 1, nivelDesc, parents);
            }
            else
            {
                printf("No se pudo crear segundo proceso hijo en el nivel %d.\n", nivelActual);
                exit(2);
            }

            wait(NULL);

        }
        else if (hijo1 == 0)
        {
            // parents[nivelActual] = getpid();
            crearHijos(nivelActual + 1, nivelDesc, parents);
        }
        else
        {
            printf("No se pudo crear el primer proceso hijo en el nivel %d.\n", nivelActual);
            exit(1);
        }
    }
}
