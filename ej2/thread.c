#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct
{
    int n;
    int p;
    int m;
} t_info_pthread;

void *funcion(void *arg);

int main()
{
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

    return 0;
}

void *funcion(void *arg)
{
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
}