#include "game.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include <ctype.h>

#define PALABRAS 5
#define INTENTOS 6

bool srand_called = false;

int i_rnd(int i) {
    // if (!srand_called) {
    //     srand(time(NULL) << 10);
    //     srand_called = true;
    // }
	srand(time(NULL));
    return rand() % i;
}

char* printBody(int errores, char* body)
{
	char bodyResult[900];

	switch(errores) {

		case 6: body[6] = '\\'; break;
		case 5: body[5] = '/'; break;
		case 4: body[4] = '\\'; break;
		case 3: body[3] = '|'; break;
		case 2: body[2] = '/'; break;
		case 1: body[1] = ')', body[0] = '('; break;
		default: break;

	}


	snprintf(bodyResult, sizeof(bodyResult), "\tErrores :%d\n"
		   "\t _________\n"
	       "\t|         |\n" 
	       "\t|        %c %c\n"
	       "\t|        %c%c%c\n"
	       "\t|        %c %c\n"
	       "\t|             \n"
	       "\t|             \n", errores,body[0], body[1], body[2], body[3],body[4],body[5],body[6]);


	char* pBodyResult = bodyResult;
	return pBodyResult;
}

char* printWord(char* guess, int len) 
{
	char wordResult[len];
	 strcpy(wordResult, "\0");
	  char cToStr[2];
	  cToStr[0] = ' ';  
      cToStr[1] = '\0';    

	// printf("\n%s\n", guess);
	// printf("\n%s\n", wordResult);


	for (int i = 0; i < len; ++i)
	{
            cToStr[0] = guess[i];    

		strcat(wordResult, cToStr);
	}
	strcat(wordResult,"\n\n");

	char* pwordResult = wordResult;
	return pwordResult;
}

char* getWord()
{
	//Vamos a buscar la palabra a adivinar desde el archivo
	int indexPalabra = i_rnd(PALABRAS);

	char * fileName = "palabras"; 
    FILE* file = fopen(fileName, "r"); 
    // char line[256];
    char line[256];
	char *pline = line;
	int lineas = 0;

    while (fgets(line, sizeof(line), file) && indexPalabra != lineas) {
		lineas ++;
    }

    fclose(file);
 	char* p;

	if ((p = strchr(line, '\n')) != NULL){
		// printf("reemplace");
		p--;
		*p = '\0';
	}

	// printf("%s \n", line);

	return pline;
}

void tryLetter(char* word, int len, char* guessed, char* falseWord, int* errores, char guess)
{
	bool found = false;

	printf("word: %s - len: %d - guessed: %s - falseWord: %s - errores: %d - guess: %c\n", 
			word, len, guessed, falseWord, *errores, guess);

	guess = tolower(guess);

		for (int i = 0; i < len; ++i)
		{
			if(word[i] == guess) {
				found = true;
				guessed[i] = guess;
			}	
		}
		if(!found) {
			falseWord[*errores] = guess;
			*errores += 1;
		}
}

