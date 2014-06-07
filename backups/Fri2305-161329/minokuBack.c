#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "minokuBack.h"
#include "misc.h"

int setDificultad(TipoParametros * parametros, int casilleros)
{
	switch(parametros -> dificultad)
	{
		case 1: 
			parametros -> cantUndo = U_FACIL;
			parametros -> cantMinas = casilleros * PORCENTAJE(P_FACIL);
			break;
		case 2: 
			parametros -> cantUndo = U_MEDIO;
			parametros -> cantMinas = casilleros * PORCENTAJE(P_MEDIO);
			break;
		case 3: 
			parametros -> cantUndo = U_DIFICIL;
			parametros -> cantMinas = casilleros * PORCENTAJE(P_DIFICIL);
			break;
		case 4: 
			parametros -> cantUndo = U_PESADILLA;
			parametros -> cantMinas = casilleros * PORCENTAJE(P_PESADILLA);
			break;
		default:
			return ERROR;
	}
	parametros -> flags = parametros -> cantMinas;
	return OK;
}

int prepararTableros(TipoTablero * incognita, TipoTablero * tablero, int filas, int columnas, TipoParametros * parametros)
{
	*incognita = generarTablero(filas, columnas);
	*tablero = generarTablero(filas, columnas);
	if(*incognita == NULL || *tablero == NULL)
		return ERROR;
	llenarTablero(*tablero, filas, columnas, NO_TOCADO);
	llenarTablero(*incognita, filas, columnas, VACIO);
	llenarMinas(*incognita, filas, columnas, parametros);
	return OK;
}

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

void guardarDatos(FILE *fp, TipoTablero incognita, TipoTablero tablero, int filas, int cols, TipoParametros * p)
{
	int i;

	fwrite(&(p->dificultad), sizeof(int), 1, fp);
	fwrite(&filas, sizeof(int), 1, fp);
	fwrite(&cols, sizeof(int), 1, fp);
	fwrite(&(p->movimientos), sizeof(p->movimientos), 1, fp);
	fwrite(&(p->campania), sizeof(p->campania), 1, fp);
	
	for(i = 0; i < filas; i++)
		fwrite(incognita[i], sizeof(char), cols, fp);
	for(i = 0; i < filas; i++)
		fwrite(tablero[i], sizeof(char), cols, fp);
	
	//if(campania) FALTA COMPLETAR
		//for(i = 0; i < filas; i++)
			//fwrite(fileCampania, sizeof(char), cols, fp);
}
 
int cargarDatos(FILE *fp, TipoTablero * incognita, TipoTablero * tablero, int * filas, int * columnas, TipoParametros * p)
{
	int i;
	 
	fread(&(p->dificultad), sizeof(p->dificultad), 1, fp);

	fread(filas, sizeof(*filas), 1, fp);
	fread(columnas, sizeof(*columnas), 1, fp);
	*incognita = generarTablero(*filas, *columnas);
	*tablero = generarTablero(*filas, *columnas);
	if(*incognita == NULL || *tablero == NULL)
		return ERROR;

	fread(&(p->movimientos), sizeof(p->movimientos), 1, fp);
	fread(&(p->campania), sizeof(int), 1, fp);

	setDificultad(p, (*filas) * (*columnas));

	for(i = 0; i < *filas; i++)
		fread((*incognita)[i], sizeof(char), *columnas, fp);
	for(i = 0; i < *filas; i++)
		fread((*tablero)[i], sizeof(char), *columnas, fp);

	//if(campania)
		//fread(sFile, sizeof(char), 20, fp);

	return OK;
}

void llenarMinas(TipoTablero tablero, int filas, int columnas, TipoParametros *parametros) 
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

void llenarTablero(TipoTablero tablero, int filas, int columnas, char obj) 
{
	int i, j;

	for(i = 0; i < filas; i++) 
		for(j = 0; j < columnas; j++) 
			tablero[i][j] = obj;
}

TipoTablero copiaTablero(TipoTablero tablero, int filas, int columnas) 
{
	TipoTablero auxTablero;
	int i, j;
	
	auxTablero = generarTablero(filas, columnas);

	if (auxTablero != NULL)
	{
		for(i = 0; i < filas; i++) 
			for(j = 0; j < columnas; j++) 
				auxTablero[i][j] = tablero[i][j];
	}
	return auxTablero;
}

// Validar el rango y que inicial no sea mayor que el final
// Hay que verificar que el/los casilleros no esten ya barridos
void flag(TipoTablero tablero, TipoPos inicial, TipoPos final, TipoParametros *parametros) 
{
	int i = inicial.fil;
	int j = inicial.col;
	int cant = (final.fil - inicial.fil) + 1;
	int doFil = TRUE;
	int flagueados = 0;

	if(cant == 0) 
	{
		doFil = FALSE;
		cant = (final.col - inicial.col) + 1;
	}

	while(cant--) 
	{
		if(tablero[i][j] != VACIO) 
		{
			tablero[i][j] = FLAG;
			flagueados++;
		}
		if(doFil)
			j++;
		else
			i++;
	}

	parametros -> flags -= flagueados;
}

// Hay que comprobar de antemano que haya por lo menos 1 flag en el intervalo solicitado
// Si no lo hay entonces vuelve a pedir el codigo
void unflag(TipoTablero tablero, TipoPos inicial, TipoPos final, TipoParametros *parametros) 
{
	int i = inicial.fil;
	int j = inicial.col;
	int cant = (final.fil - inicial.fil) + 1;
	int doFil = TRUE;
	int flagueados = 0;

	if(cant == 0) 
	{
		doFil = FALSE;
		cant = (final.col - inicial.col) + 1;
	}

	while(cant--) 
	{
		if(tablero[i][j] == FLAG) 
		{
			tablero[i][j] = NO_TOCADO;
			flagueados++;
		}
		if(doFil)
			j++;
		else
			i++;
	}

	parametros -> flags += flagueados;
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
	tablero[i][j] = VACIO;
	return 1;
}

int * query(TipoTablero incognita, int filas, int columnas, int num, char doFil)
{
	int i = num, j = 0, dim = 0;
	int * positions = NULL;
	char habiaMina = FALSE;

	if(!doFil) 
	{
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
	positions[dim] = EOA;
	
	return positions;
}

void liberarMatriz(TipoTablero matriz, size_t filas) 
{
	int i;

	for (i = 0; i < filas; i++)
		free(matriz[i]);
	free(matriz);
}

void * reallocVector(void *vector, int dim, size_t size)
{
	vector=realloc(vector, (dim + BLOQUE) * size);
	if (vector == NULL)
		free(vector);
	return vector;
}