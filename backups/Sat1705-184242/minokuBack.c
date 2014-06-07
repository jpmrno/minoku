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

void llenarTablero(TipoTablero tablero, int filas, int columnas, TipoParametros *parametros) 
{
	int i;
	int randFil, randCol;

	for( i = 0; i < parametros -> cantMinas; ) 
	{
		randFil = randInt(0, filas - 1);
		randCol = randInt(0, columnas - 1);
		if(tablero[randFil][randCol] != MINA) 
		{
			tablero[randFil][randCol] = MINA;
			i++;
		}
	}
}

void liberarMatriz(TipoTablero matriz, size_t filas) 
{
	int i;

	for (i = 0; i < filas; i++)
		free(matriz[i]);
	free(matriz);
}

// Validar el rango y que inicial no sea mayor que el final
// Hay que verificar que el/los casilleros no esten ya barridos
void flag(TipoTablero tablero, TipoPos inicial, TipoPos final) 
{
	int i = inicial.fil;
	int j = inicial.col;
	int cant = (final.fil - inicial.fil) + 1;
	int doFil = TRUE;

	if(cant == 0) {
		doFil = FALSE;
		cant = (final.col - inicial.col) + 1;
	}

	while(cant--) {
		if(tablero[i][j] == FLAG)
			tablero[i][j] = VACIO;
		if(doFil)
			j++;
		else
			i++;
	}
}

// Hay que comprobar de antemano que haya por lo menos 1 flag en el intervalo solicitado
// Si no lo hay entonces vuelve a pedir el codigo
void unflag(TipoTablero tablero, TipoPos inicial, TipoPos final) 
{
	int i = inicial.fil;
	int j = inicial.col;
	int cant = (final.fil - inicial.fil) + 1;
	int doFil = TRUE;

	if(cant == 0) {
		doFil = FALSE;
		cant = (final.col - inicial.col) + 1;
	}

	while(cant--) {
		if(tablero[i][j] == FLAG)
			tablero[i][j] = VACIO;
		if(doFil)
			j++;
		else
			i++;
	}
}

int barrerCasillero(TipoTablero incognita, TipoTablero tablero, TipoPos elemento) 
{
	int i = elemento.fil;
	int j = elemento.col;

	if(incognita[i][j] == MINA) 
	{
		tablero[i][j] = MINA;
		return 0;
	}
	tablero[i][j] = BARRIDO;
	return 1;
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

void *reallocVector(void *vector, size_t size)
{
	vector=realloc(vector, (dim+BLOQUE)*size);
	if (vector == NULL)
		free(vector);
	return vector;
}