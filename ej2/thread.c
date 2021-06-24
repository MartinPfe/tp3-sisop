//***********************
// Nombre del Script:            ./thread
// Trabajo Practico Nro:         3
// Ejercicio Nro:                2
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


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <string.h>

typedef struct
{
    int n;
    int p;
    int m;
} t_info_pthread;

void *process(void *arg);

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        if (argc == 2 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0))
        {
            printf("Ha ingresado a la seccion ayuda:\nEste programa recibe 2 parametros: un número N (cantidad de iteraciones), y un númeroP (nivel de paralelismo).\nDe esta manera, dado el intervalo [2-9] se ejecutarán una serie de operaciones matemáticas sobre cadauno de estos numeros pertenecientes al intervalo. Finalmente se informara cuanto tiempo demoro en ejecutar las operaciones matematicascorrespondientes a cada numero del intervalo, y el tiempo total demorado\n");
            return 1;
        }

        printf("Parametros incorrectos. Ingrese 2 paramtros.\nPara consultar ayuda ejecute con el parametro '-h' o '--help'\n");
        return 2;
    }

    if (atoi(argv[1]) < 1)
    {
        printf("Error: La cantidad de iteraciones debe ser mayor o igual a 1.\n");
        return 3;
    }

    if (atoi(argv[2]) < 1)
    {
        printf("Error: El nivel de paralelismo debe ser mayor o igual a 1.\n");
        return 4;
    }

    int it = atoi(argv[1]);
    int paral = atoi(argv[2]);
    int i, j;

    // printf("Por favor, ingrese cantidad de iteraciones:\n");
    // scanf("%d", &it);

    // if (it < 1)
    // {
    //     printf("Numero de iteraciones invalido. Por favor ingrese un numero mayor o igual a 1\n");
    //     return 1;
    // }

    // printf("Por favor, ingrese nivel de paralelismo:\n");
    // scanf("%d", &paral);

    // if (paral < 1)
    // {
    //     printf("Nivel de paralelismo invalido. Por favor ingrese un nivel mayor o igual a 1\n");
    //     return 2;
    // }

    struct timeval start, end;
    gettimeofday(&start, NULL);

    pthread_t thread[paral];

    t_info_pthread *info;

    for (i = 0; i < paral; i++)
    {
        info = malloc(sizeof(t_info_pthread *));
        if (info == NULL)
        {
            printf("Error: No se pudo reservar memoria para el Thread %d\n", i + 1);
            return 5;
        }

        info->n = it;
        info->p = paral;
        info->m = i;

        pthread_create(&thread[i], NULL, &process, info);
    }

    for (j = 0; j < paral; j++)
        pthread_join(thread[j], NULL);

    gettimeofday(&end, NULL);

    long seconds = (end.tv_sec - start.tv_sec);
    long micros = ((seconds * 1000000) + end.tv_usec) - (start.tv_usec);

    printf("El tiempo final transcurrido fue de %ld segundos con %ld microsegundos\n", seconds, micros);

    return 0;
}

void *process(void *arg)
{
    struct timeval starts, ends;

    t_info_pthread data = *(t_info_pthread *)arg;

    int i, j, acum;

    for (i = 2 + data.m; i <= 9; i += data.p)
    {
        gettimeofday(&starts, NULL);

        acum = i;

        for (j = 0; j < data.n; j++)
        {
            acum *= i;
            acum += acum;
            acum /= data.n;
        }

        gettimeofday(&ends, NULL);

        long seconds = (ends.tv_sec - starts.tv_sec);
        long micros = ((seconds * 1000000) + ends.tv_usec) - (starts.tv_usec);

        printf("El tiempo demorado en ejecutar el Ciclo del Numero '%d' fueron %ld segundos con %ld microsegundos\n", i, seconds, micros);
    }

    free(arg);
}