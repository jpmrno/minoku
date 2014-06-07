#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "minokuBack.h"
#include "misc.h"

TipoTablero generarTablero(int filas, int columnas) 
{
	int i;
	TipoTablero tablero;

	tablero = malloc(filas * sizeof(TipoFila));
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

int flag(TipoTablero tablero, TipoPos inicial, TipoPos final) 
{
	int i = inicial.fil;
	int j = inicial.col;
	int cant = (final.fil - inicial.fil);
	int doFil = TRUE;

	if(cant == 0) {
		doFil = FALSE;
		cant = (final.col - inicial.col);
	}

	while(cant--) {
		tablero[i][j] = FLAG;
		if(doFil)
			j++;
		else
			i++;
	}
	return 1;
}

int barrerCasillero(TipoTablero tablero, TipoPos elemento) 
{
	int i = elemento.fil;
	int j = elemento.col;

	if(tablero[i][j] == MINA) 
	{
		tablero[i][j] = MINA;
		return 0;
	}
	else
		tablero[i][j] = BARRIDO;
	return OK;
}	

void liberarMatriz(TipoTablero matriz, size_t filas) 
{
	int i;

	for (i = 0; i < filas; i++)
		free(matriz[i]);
	free(matriz);
}

int * query(TipoTablero incognita, int filas, int columnas, int num, char doFil)
{
	int i = num, j = 0, dim = 0;
	int * positions = NULL;
	char habiaMina = FALSE;

	if(!doFil) {
		j = num;
		i = 0;
	}

	while(i < filas && j < columnas) //caso de revisar de arriba pa abajo
	{
		if(dim % BLOQUE == 0) 
		{
			positions = realloc(positions, (dim + BLOQUE) * sizeof(*positions));
			if (positions == NULL)
			{
				free(positions);
				return NULL;
			}
		}
		if(incognita[i][j] == MINA) //Si encontró algo
		{	
			if(!habiaMina)
			{
				habiaMina = TRUE;
				positions[dim] = 0;
			}
			positions[dim]++;
		}
		else if(habiaMina) //Si había una MINA onteriormente pero ahora no hay nada
		{
			habiaMina = FALSE;
			dim++;
		}
		if(doFil)
			j++;
		else
			i++; 
	}
	if(habiaMina)
		dim++;
	positions = realloc(positions, dim * sizeof(*positions));
	if(positions == NULL)
	{	
		free(positions);
		return NULL;
	}
	positions[dim] = -1;
	
	return positions;
}