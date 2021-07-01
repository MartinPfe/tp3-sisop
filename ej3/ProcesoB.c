#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#define err(msg, arg){ fprintf(stderr, "%s, vea %s -h\n", msg, arg); exit(1);}
#define help(arg){ fprintf(stderr, "Ejemplo de ejecucion:\n ./path-del-ejecutable \nEjemplo: %s \n", arg); exit(1);}

int pedir_anio(), pedir_mes();
void main(int argc, char*argv[]){

	if(argc > 1 && strcmp(argv[1], "-h") == 0){
		help(argv[0]);
	}

	if(argc < 1)
		err("Argumentos insuficientes", argv[0]);

	char * pipe_path = "/tmp/fifo";
	mkfifo(pipe_path, 0666);

	char*meses[] = { "enero", "febrero", "marzo", 
				 "abril", "mayo", "junio", 
				 "julio", "agosto", "septiembre", 
				 "octubre", "noviembre", "diciembre" };

	printf("\tMenu:\n1- Facturacion Mensual.\n2- Facturacion Anual.\n3- Facturacion Media Anual.\n4- Salir.\n");		
	
	int opc, anio;
	char*mes = (char*)malloc(sizeof(char)*strlen("septiembre"));
	char* instruccion = (char*)malloc(sizeof(char)*50);
	
	do{	
		scanf("%d", &opc);
		
		if(opc > 0 && opc < 4){		
			sprintf(instruccion, "%d-%d/-%s.txt", opc, (anio = pedir_anio()), opc==1? mes = meses[pedir_mes()-1]:"");
			break;
		}

		if(opc == 4){	
			sprintf(instruccion, "%d-", opc);
			return;
		}
		printf("Opcion Invalida. Ingrese Nuevamente..\n");

	}while(1);

	int fd = open(pipe_path, O_WRONLY);
	write(fd, instruccion, strlen(instruccion));
	close(fd);
		
	float res;
	fd = open(pipe_path, O_RDONLY);
	read(fd, &res, sizeof(res));
	close(fd);

	if(res == INFINITY)
		printf("No existe facturacion correspondiente\n");
	else{
		printf("Facturacion correspondiente");
		printf(strlen(mes) != 0? " a %s del %d: ": " al %s%d:", mes,anio);
		printf(res != -1.00?" %.2f\n":"\n", res);
	}
	unlink(pipe_path);
	rmdir(pipe_path);
    return;
}

int pedir_anio(){
	int anio;
	do{
		printf("Ingrese año de facturacion: ");
		scanf("%d", &anio);
		if( anio < 0 || anio > 2021)
			printf("Año invalido. Ingrese nuevamente..\n");
		else	
			break;
	}while(1);
	return anio;
}
int pedir_mes(){
	int mes;
	do{
		printf("Ingrese mes de facturacion (Formato: MM): ");
		scanf("%d", &mes);
		if(mes < 1 || mes > 12)
			printf("Mes invaldo. Ingrese nuevamente.. \n");
		else
			break;
	}while(1);
	return mes;
}
