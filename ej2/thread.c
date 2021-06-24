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

void *funcion(void *arg);

char sTiempoEjecucion[1024] = ""; //tiempo de ejecucion


int main()
{
    // Comienza a contabilizar el tiempo de ejecucion total
    struct timeval comienzoEjecucion, finEjecucion;
    gettimeofday(&comienzoEjecucion, NULL);


    int it, paral, i, j;

    printf("Por favor, ingrese cantidad de iteraciones:\n");
    scanf("%d", &it);

    if (it < 1)
    {
        printf("Numero de iteraciones invalido. Por favor ingrese un numero mayor o igual a 1\n");
        return 1;
    }

    printf("Por favor, ingrese nivel de paralelismo:\n");
    scanf("%d", &paral);

    if (paral < 1)
    {
        printf("Nivel de paralelismo invalido. Por favor ingrese un nivel mayor o igual a 1\n");
        return 2;
    }

    pthread_t thread[paral];
    t_info_pthread info;
    info.n = it;
    info.p = paral;

    for (i = 0; i < paral; i++)
    {
        info.m = i;
        pthread_create(&thread[i], NULL, &funcion, &info);
    }

     for (j = 0; j < paral; j++)
         pthread_join(thread[j], NULL);

    // Fin de contabilizar el tiempo de ejecucion total
    gettimeofday(&finEjecucion, NULL);
    long segundos = finEjecucion.tv_sec - comienzoEjecucion.tv_sec;
    long microsegundos = finEjecucion.tv_usec - comienzoEjecucion.tv_usec;
    double tiempoTranscurrido = segundos + microsegundos*1e-6;

    
    // printf("Tiempo total de ejecucion: %.3f segundos.\n", tiempoTranscurrido);
    char sTiempoTotal[100] = "";
    snprintf(sTiempoTotal, sizeof(sTiempoTotal), "Tiempo total de ejecucion: %.3f segundos.\n", tiempoTranscurrido);
    strcat(sTiempoEjecucion, sTiempoTotal);
    puts(sTiempoEjecucion);

    return 0;
}

void *funcion(void *arg)
{
    // Comienza a contabilizar el tiempo de ejecucion del ciclo

    struct timeval comienzoEjecucion, finEjecucion;
    gettimeofday(&comienzoEjecucion, NULL);

    t_info_pthread data = *(t_info_pthread *)arg;

    int i, j, acum;

    for (i = 2 + data.m; i <= 9; i += data.p)
    {
        acum = i;

        for (j = 0; j < data.n; j++)
        {
            acum *= i;
            acum += acum;
            acum /= data.n;
            printf("Iteracion %d con M= %d\n", j + 1, i);
        }
    }

    // Fin de contabilizar el tiempo de ejecucion total
    gettimeofday(&finEjecucion, NULL);
    long segundos = finEjecucion.tv_sec - comienzoEjecucion.tv_sec;
    long microsegundos = finEjecucion.tv_usec - comienzoEjecucion.tv_usec;
    double tiempoTranscurrido = segundos + microsegundos*1e-6;

    char sTiempoCiclo[100] = "";
    snprintf(sTiempoCiclo, sizeof(sTiempoCiclo), "Tiempo total del ciclo: %.3f segundos.\n", tiempoTranscurrido);
    strcat(sTiempoEjecucion, sTiempoCiclo);
}