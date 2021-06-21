#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include <ctype.h>

#define PALABRAS 5
#define INTENTOS 6

char* getWord();
void tryLetter(char* word, int len, char* guessed, char* falseWord, int* errores, char guess);
char* printWord(char* guess, int len);
char* printBody(int errores, char* body);
