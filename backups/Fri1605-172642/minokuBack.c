#include <stdio.h>
#include <stdlib.h>
#include "minokuBack.h"
#include "misc.h"

TipoTablero generarTablero(int filas, int columnas) 
{
	int i;
	TipoTablero tablero;

	tablero = malloc(filas * sizeof(TipoLinea));
	if(tablero == NULL)
		return NULL;
	for( i = 0; i < filas; i++) {
		tablero[i] = malloc(columnas * sizeof(TipoTablero));
		if(tablero[i] == NULL) {
			liberarMatriz(tablero, i);
			return NULL;
		}
	}
	return tablero;
}

void llenarTablero(TipoTablero tablero, int filas, int columnas, int cantMinas) 
{
	int i;
	int randFil, randCol;

	for( i = 0; i < cantMinas; ) 
	{
		randFil = randInt(0, filas - 1);
		randCol = randInt(0, columnas - 1);
		//Para ver los rangos que tira (si estan bien)
		//printf("%d\n", randFil);
		//printf("%d\n", randCol);
		if(tablero[randFil][randCol] != MINA) 
		{
			tablero[randFil][randCol] = MINA;
			i++;
		}
	}
}

int flag(TipoTablero tablero, TipoPos inicial, int cant, char filOcol) 
{
	int i = inicial.i;
	int j = inicial.j;

	if(inicial.i == final.i)
		filOcol = 'c';
	else
		filOcol = 'f';

	while(cant--) {
		tablero[i][j] = FLAG;
		if(filOcol == 'f')
			j++;
		else
			i++;
	}
	return 1;
}

// No se si vale la pena tenerla
int tieneMina(const TipoTablero incognita, int fila, int columna)
{
	return incognita[fila][columna] == MINA;
}	

void liberarMatriz(TipoTablero matriz, size_t filas) 
{
	int i;

	for (i = 0; i < filas; i++)
		free(matriz[i]);
	free(matriz);
}
