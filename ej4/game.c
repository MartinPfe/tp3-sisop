/**
* @autor Saroj Tripathi
* @name Hangman
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include <ctype.h>

#define WORDS 10
#define WORDLEN 40
#define INTENTOS 6

bool srand_called = false;

int i_rnd(int i) {
    if (!srand_called) {
        srand(time(NULL) << 10);
        srand_called = true;
    }
    return rand() % i;
}

void printBody(int errores, char* body) {
	printf("\tErrores :%d\n", errores);
	switch(errores) {

		case 6: body[6] = '\\'; break;
		case 5: body[5] = '/'; break;
		case 4: body[4] = '\\'; break;
		case 3: body[3] = '|'; break;
		case 2: body[2] = '/'; break;
		case 1: body[1] = ')', body[0] = '('; break;
		default: break;

	}

	printf("\t _________\n"
	       "\t|         |\n"
	       "\t|        %c %c\n"
	       "\t|        %c%c%c\n"
	       "\t|        %c %c\n"
	       "\t|             \n"
	       "\t|             ", body[0], body[1], body[2],
	       body[3], body[4], body[5], body[6]);
}

void printWord(char* guess, int len) {
	printf("\t");
	for (int i = 0; i < len; ++i)
	{
		printf("%c ", guess[i]);
	}
	printf("\n\n");
}

int main() {

	printf("\n\t Be aware you can be hanged!!.");

	printf("\n\n\t Rules : ");
	printf("\n\t - Maximum 6 mistakes are allowed.");
	printf("\n\t - All alphabet are in lower case.");
	printf("\n\t - All words are name of very popular Websites. eg. Google");
	printf("\n\t - If you enjoy continue, otherwise close it.");

	printf("\n\t Syntax : Alphabet");
	printf("\n\t Example : a \n\n");
	char *body = malloc(INTENTOS+1);

	int id = i_rnd(WORDS);
	char *word = "google";
	int len = strlen(word);
	char *guessed = malloc(len);
	char falseWord[INTENTOS];

	memset(body, ' ', INTENTOS+1);
	memset(guessed, '_', len);
	char guess;
	bool found;
	char* win;

	int errores = 0;
	setvbuf(stdin, NULL, _IONBF, 0);

	do {

		found = false;
		printf("\n\n");
		printBody(errores, body);
		printf("\n\n");
		printf("\tFalse Letters : ");
		if(errores == 0) printf("None\n");
		for (int i = 0; i < errores; ++i)
		{
			printf("%c", falseWord[i]);
		}
		printf("\n\n");
		printWord(guessed, len);
		printf("\tGive me a alphabet in lower case : ");
		do {scanf("%c",&guess);} while ( getchar() != '\n' );

        guess = tolower(guess);
		for (int i = 0; i < len; ++i)
		{
			if(word[i] == guess) {
				found = true;
				guessed[i] = guess;
			}	
		}
		if(!found) {
			falseWord[errores] = guess;
			errores += 1;
		}
		win = strchr(guessed, '_');
	}while(errores < INTENTOS && win != NULL);

	if(win == NULL) {
		printf("\n");
		printWord(guessed, len);
		printf("\n\tCongrats! You have won : %s\n\n", word);
	} else {
		printf("\n");
		printBody(errores, body);
		printf("\n\n\tBetter try next time. Word was %s\n\n", word);
	}

	free(body);
	free(guessed);
	return EXIT_SUCCESS;
}