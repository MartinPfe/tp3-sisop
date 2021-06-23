#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <ctype.h>

void funcion(int num,int vec[],int vec2[],int num2);
void ayuda();
int main(int argc, char*argv[])
{
 if(argc<=1){
    printf("ERROR, no ha ingresado un numero\n");
    exit(0);
    }
 if(argc>2){
    printf("ERROR, ha ingresado mas de un parametro\n");
    exit(1);
    }
 if(strcmp(argv[1],"-h")==0||strcmp(argv[1],"-H")==0||strcmp(argv[1],"-help")==0||strcmp(argv[1],"-HELP")==0||strcmp(argv[1],"-?")==0){
    ayuda();
    exit(3);
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
 printf("el nivel ingresado es:%d\n",valor);
 if(valor<=0){
    printf("ERROR, ha ingresado un numero negativo o cero\n");
    exit(6);
    } 
 
 int num=valor;
 pid_t pidvec[num+1];
 int num2=1;
 int num3=4;
 int vec[4+num];
 int vecPro[num+1];
 int potencia;
 int status;

 for(int i=1;i<=num;i++){
 	potencia=(i-1);
 	vecPro[i]=1;
   while (potencia>-1){
    	vecPro[i]=(2*vecPro[i]);
    	potencia--;
   }
   vecPro[i]=vecPro[i]-1;
 }
 //printf("0(%d)\n",getpid());
pidvec[1]=getpid();
printf("Proceso 1: (%d)\n",pidvec[1]);
if (num==1){
    exit(7);
    }
else{
	printf("el nivel %d: tiene %d procesos\n",num,vecPro[num]);
	for(int i=2;i<=3;i++){
		//printf("el nivel %d: tiene %d procesos\n",i,vecPro[i]);
		if(!fork()){
			pidvec[i]=getpid();
      	printf("Proceso %d: (%d)-Proceso Padre:(%d)\n",i,pidvec[i],getppid());         
			if(num==2) {
				//wait(&status);				
				exit(7);
    			}
    		else {			
				for (int j=4;j<=5;j++){       
					if(!fork()){
						if(i==2) {
							pidvec[j]=getpid();
         				printf("Proceso %d: (%d)-Proceso Padre:(%d)\n",j,pidvec[j],getppid());
         				}
         			else {
         				j=j+2;
         				pidvec[j]=getpid();
         				printf("Proceso %d: (%d)-Proceso Padre:(%d)\n",j,pidvec[j],getppid());
         				}
         			if(num==3) {				    		
				    		exit(7);
    					}
         		//exit(7);
   				}
      			//exit(7);
      		}
      		//exit(7);
      		wait(&status);
			}
			//exit(7);
		}
		wait(&status);
	}
}
return 0;
}
void funcion(int num,int vec[],int vec2[],int num3){
  pid_t pid1;
    if(num==0){
        return;
    }
    else{
        if(!fork()){
            pid1=getpid();
            vec[num3]=pid1;
            for(int i=num3;i>=0;i--){
                if(i==0){
                    printf("%d(%d)",vec2[i],vec[i]);
                }else{
                    printf("%d(%d)-",vec2[i],vec[i]);
                }

            }
            printf("\n");
            num3++;
            funcion(num-1,vec,vec2,num3);
            exit(0);
        }
    }
}
void ayuda(){
    printf("Ingrese un numero N para crear N generaciones de procesos\n");
    printf("Ejemplo de ejecucion:\n");
    printf("1- Make ej1 \n");
    printf("2- ./ej1 5 \n");
}
