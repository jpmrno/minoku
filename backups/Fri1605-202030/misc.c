#include <stdio.h>
#include <stdlib.h>
#include "misc.h"

double randNormalize(void)
{
	return rand() / ((double) RAND_MAX + 1);
}

int randInt( int izq, int der )
{
	return	(randNormalize() * (der - izq + 1) + izq);
}

void printa(const char s[]) 
{
	printf("\n~~~ %s ~~~\n\n", s);
}