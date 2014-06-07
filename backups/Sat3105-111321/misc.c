#include <stdio.h>
#include <stdlib.h>
#include "misc.h"
#include "minokuBack.h"

double randNormalize(void)
{
	return rand() / ((double) RAND_MAX + 1);
}

int randInt(int izq, int der)
{
	return	(randNormalize() * (der - izq + 1) + izq);
}

void printa(const char s[]) 
{
	printf("\n~~~ %s ~~~\n\n", s);
}

void printError(int error) 
{
	switch(error)
	{
		case ERROR:	
			printa("Hubo un error");
			break;
		case ERROR_MALLOC:	
			printa("No en la (re)alocacion de memoria");
			break;
		case ERROR_CORRUPT_FILE:
			printa("El archivo esta corrupto");
			break;
		case ERROR_OPEN_FILE:	
			printa("El archivo no existe");
			break;
		case ERROR_NAME_FILE:	
			printa("El nombre del archivo es invalido");
			break;
	}
}