#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
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

int prepararJuego(TipoTablero * incognita, TipoTablero * tablero, int filas, int columnas, TipoParametros * parametros)
{
	*incognita = generarTablero(filas, columnas);
	*tablero = generarTablero(filas, columnas);
	if(*incognita == NULL || *tablero == NULL)
		return ERROR_MALLOC;

	setDificultad(parametros, (filas * columnas));

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

int guardarDatos(TipoTablero incognita, TipoTablero tablero, int filas, int columnas, TipoParametros * p, char * fileCamp, char * fileSave)
{
	int i;
	FILE *fp;

	fp = fopen(fileSaved, "wb");
	if(fp == NULL)
		return ERROR_NAME_FILE;

	fwrite(&(p->dificultad), sizeof(p->dificultad), 1, fp);

	fwrite(&filas, sizeof(filas), 1, fp);
	fwrite(&columnas, sizeof(columnas), 1, fp);

	fwrite(&(p->cantUndo), sizeof(p->cantUndo), 1, fp);
	fwrite(&(p->movimientos), sizeof(p->movimientos), 1, fp);
	fwrite(&(p->campania), sizeof(p->campania), 1, fp);
	
	for(i = 0; i < filas; i++)
		fwrite(incognita[i], sizeof(**incognita), columnas, fp);
	for(i = 0; i < filas; i++)
		fwrite(tablero[i], sizeof(**tablero), columnas, fp);
	
	if(p->campania)
		fwrite(fileCamp, sizeof(*fileCamp), sizeof(fileCamp), fp);

	fclose(fp);

	return OK;
}

int cargarDatos(TipoTablero * incognita, TipoTablero * tablero, int * filas, int * columnas, TipoParametros * p, char * fileCamp, char * fileSaved)
{
	int i, leidos = 0;
	char * aux;
	FILE * fp;

	fp = fopen(fileSaved, "rb");
	if(fp == NULL)
		return ERROR_OPEN_FILE;

	// Comienzo a leer los datos
	leidos += fread(&(p->dificultad), sizeof(p->dificultad), 1, fp);
	leidos += fread(filas, sizeof(*filas), 1, fp);
	leidos += fread(columnas, sizeof(*columnas), 1, fp);
	leidos += fread(&(p->cantUndo), sizeof(p->cantUndo), 1, fp);
	leidos += fread(&(p->movimientos), sizeof(p->movimientos), 1, fp);
	leidos += fread(&(p->campania), sizeof(p->campania), 1, fp);
	
	// Compruebo que lo leido hasta ahora este bien
	if(leidos != 6 || !setDificultad(p, *filas * *columnas) || !validarCarga(filas, columnas, p->cantUndo, p->movimientos))
		return ERROR_CORRUPT_FILE;

	// Genero las matrices para poder cargar sus datos guardados
	*incognita = generarTablero(*filas, *columnas);
	*tablero = generarTablero(*filas, *columnas);
	if(*incognita == NULL || *tablero == NULL)
		return ERROR_MALLOC;

	for(i = 0; i < *filas; i++) 
	{
		leidos = fread((*incognita)[i], sizeof(***incognita), *columnas, fp);
		if(leidos != *columnas) {
			FALTA LIBERAR MATRICES
			return ERROR_CORRUPT_FILE;
		}
	}
	for(i = 0; i < *filas; i++)
	{
		leidos = fread((*tablero)[i], sizeof(***tablero), *columnas, fp);
		if(leidos != *columnas) {
			FALTA LIBERAR MATRICES
			return ERROR_CORRUPT_FILE;
		}
	}

	// Si es campaña, entonces cargo el nombre del archivo
	if(p->campania) 
	{
		leidos = 0;
		while(!feof) {
			if(leidos % BLOQUE == 0)
			{
				aux = realloc(fileCamp, (leidos + BLOQUE) * sizeof(*fileCamp));
				if(aux == NULL)
				{
					free(fileCamp);
					return ERROR_MALLOC;
				}
				fileCamp = aux;
			}
			leidos += fread(fileCamp, sizeof(*fileCamp), BLOQUE, fp);
		}
		if(leidos == 0)
			return ERROR_CORRUPT_FILE;
		fileCamp = realloc(fileCamp, leidos * sizeof(*fileCamp));
	} else
		if(!feof(fp)) // Compruebo que cuando termine de leer todo, no queden mas cosas en el archivo
			return ERROR_CORRUPT_FILE;

	fclose(fp);

	return OK;
}

int validarCarga(int filas, int columnas, int cantUndo, int movimientos) 
{
	if(filas < MIN_FIL || filas > MAX_FIL)
		return ERROR;

	if(columnas < MIN_COL || columnas > MAX_COL)
		return ERROR;

	if(cantUndo < 0)
		return ERROR;

	if(movimientos <= 0)
		return ERROR;

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

TipoUndo copiadoPreventivo(TipoTablero tablero, int filas, int columnas, int * flagsTotales, int * flagsCorrectos, int * sCorrectos) 
{
	static TipoUndo estado;

	estado.tablero = copiaTablero(tablero, filas, columnas);
	estado.flagsTotales = flagsTotales;
	estado.flagsCorrectos = flagsCorrectos;
	estado.sCorrectos = sCorrectos;

	return &estado;
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

// Hay que verificar que el/los casilleros no esten ya barridos
void flag(TipoTablero tablero, TipoPos inicial, TipoPos final, TipoParametros *parametros) 
{
	int i = inicial.fil - 'A';
	int j = inicial.col;
	int cant = (final.fil - inicial.fil) + 1;
	int doFil = TRUE;
	int flagueados = 0;

	if(cant == 1) 
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

	if(cant == 1) 
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
	vector = realloc(vector, (dim + BLOQUE) * size);
	if (vector == NULL)
		free(vector);
	return vector;
}

char * leerString(void)
{
	char * string = NULL, * aux;
	char c;
	int dim = 0;

	while((c = getchar()) != '\n')
	{
		if(dim % BLOQUE == 0)
		{
			aux = realloc(string, (dim + BLOQUE) * sizeof(*string));
			if(aux == NULL)
			{
				free(string);
				return NULL;
			}
			string = aux;
		}
		string[dim++] = c;
	}
	string[dim] = '\0';
	return string;
}

int validarComando(char * comando, TipoTablero incognita, TipoTablero tablero, int, filas, int columnas, TipoParametros parametros)
{
	char espacio, primerCar, posX1, posY1;
	int posY1, posY2;
	char estado;
	char * saveFile;

	switch(*comando)
	{
		case 's':	/*Está bien if((sscanf(comando, "save%c%c", &espacio, &primeraLetra) == 2)
						if(espacio==' ' && primerCar!=' ' && primeraLetra!='\0')
						{
							saveFile = mallloc(saveFile, strlen(comando+4));
							if(saveFile != NULL)
							{
								strcpy(saveFile, comando+5);
								estado = guardarDatos(incognita, tablero, filas, columnas, parametros, )
								return estado;
							}
							return ERROR_MALLOC;
						}
						else
							return ERROR;
					}
					*/
/*Sweep*/			else if(sscanf(comando, "s%c(%c,%d)", &espacio, &posX1, &posY1) == 3)
						if(espacio == ' ' && withinRange(toupper(posX1)-'A', posY1, filas, columnas))	
							return SWEEP_C;
					return ERROR;
					break;
		case 'q': 	
			if(strcmp(comando, "quit")) //Falta ver que pueda llegar a ser query
				return QUIT_C;
			else if(sscanf(comando, "query%c%d", &espacio, &primerCar) == 2)
				break;
		case 'f':
			if(sscanf(comando, "flag%c(%c,%d)"), &espacio, &posX1, &posY1)
			{

			}
			else if(sscanf(comando, "flag%c(%c,%d:%c,%d)", &espacio))
			{

			}
		case 'u':
			if(strcmp(comando, "undo"))
					return UNDO_C;
		default:	
			return ERROR;
	}
}

int interpretarComando(char * funcion, char * parametros) 
{
	TipoPos inicial, final;

	if(!strcmp(funcion, "s") || !strcmp(funcion, "S")) 
	{
		
	}

	if(!strcmp(funcion, "flag") || !strcmp(funcion, "FLAG")) 
	{
		interpretarIntervalo(&inicial, &final);
	}

	if(!strcmp(funcion, "unflag") || !strcmp(funcion, "UNFLAG")) 
	{
		interpretarIntervalo(&inicial, &final);
	}

	if(!strcmp(funcion, "undo") || !strcmp(funcion, "UNDO")) 
	{
		
	}

	if(!strcmp(funcion, "") || !strcmp(funcion, "")) 
	{
		
	}
}

int interpretarIntervalo() 
{

}

int withinRange(int posX, int posY, int filas, int columnas)
{
	return posX>=1 && posX<=filas && posY>=1 && posY<=columnas;
}