//***********************
// Nombre del Script:            ./ProcesoA
// Trabajo Practico Nro:         3
// Ejercicio Nro:                3
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
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <dirent.h>

#define err(msg, arg){ fprintf(stderr, "%s, vea %s -h\n", msg, arg); exit(1);}

#define help(arg) { fprintf(stdout, "Ejemplo de ejecucion:\n ./path-del-ejecutable path-del-fifo carpeta-de-facturacion/\nEjemplo: %s pipe/fifo facturacion/\n", arg); exit(1);}
float facturacion(const char*, const char*);
float facturacion_mensual(const char*);
float facturacion_anual_o_media(const char*, int);

void main(int argc, char*argv[]){

	if(argc > 1 && strcmp(argv[1], "-h") == 0){
		help(argv[0]);
	}

	if(argc < 3)
		err("Argumentos insuficientes", argv[0]);
	
	char *instruccion = (char*)malloc(sizeof(char)*50);

	int fd = open(argv[1], O_RDONLY);
	read(fd, instruccion, sizeof(instruccion)*50);
	close(fd);

	const char opc = strtok(instruccion, "-")[0];
	
	if(opc - '0' == 4)
		return;
	
	const char* anio = strtok(NULL, "-");
	const char* mes = strtok(NULL, "-");

	char*folder_path = (char*)malloc(sizeof(char)*(strlen(argv[2]) + strlen(anio) + strlen(mes)));

	folder_path = argv[2];
	strcat(folder_path, anio);

	float res;
	if(opc - '0' == 1)
		res = facturacion_mensual(strcat(folder_path, mes));
	else
		res = facturacion_anual_o_media(folder_path, opc - '0');

	fd = open(argv[1], O_WRONLY);
	write(fd, &res, sizeof(res));
	close(fd);
	return;
}

float facturacion_mensual(const char* path_name){
	
	float total = 0, value;
	FILE*arch = fopen(path_name, "rt");

	if(!arch){
		return 0;
	}
	
	while(fscanf(arch, "%f\n", &value) != EOF){
		total += value;
	}
	return total;
}

float facturacion_anual_o_media(const char* folder_path, int opc){

	float total = 0;	
	int count = 0;
	
	DIR *d;
	struct dirent *folder;
	d = opendir(folder_path);

	if(d){
		while((folder = readdir(d)) != NULL){
			if( folder->d_type == DT_REG ){
				
				count++;
				char path_name[strlen(folder_path) + 10];
				strcpy(path_name, folder_path);
				strcat(path_name, folder->d_name);
			
				FILE*arch = fopen(path_name, "rt");
			
				if(!arch)
					return -1.00;
			
				float value;
				while(fscanf(arch, "%f\n", &value) != EOF){
					total += value;
				}
				fclose(arch);
			}
		}	
		closedir(d);	
	}
	else
		return 0;
	return opc == 2? total:total/count;
}
