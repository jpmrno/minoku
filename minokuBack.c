#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "minokuBack.h"

//// DECLARACIONES ////

// Manejo de matrices //

// Genera una matriz dinámica de nro. filas por nro. columnas
static tTablero generarTablero(int filas, int columnas);
// Coloca en todas las posiciones el objeto (obj) indicado por parámetro
static void llenarTablero(tTablero tablero, int filas, int columnas, char obj);
// En un juego nuevo, distribuye de manera pseudoaleatoria las minas en un tablero
static void llenarMinas(tTablero tablero, int filas, int columnas, tParametros *parametros);

// Validaciones de filas y columnas //

// Verifica que el elemento este dentro del rango de filas y de columnas
static int posValida(tElemento * elemento, int filas, int columnas);
// Valida el rango de lo ingresado en comandos específicos. En caso de que se hayan pasado los valores al revés, se intercambian
static int rangoValido(tElemento * inicial, tElemento * final, int filas, int columnas);

// Generador de número pseudoaleatorio //

// Distribuye de manera uniforme la generación de números pseudoaleatorio elegidos
static double randNormalize(void);
static int randInt(int izq, int der);

// Comandos del juego //

static int flag(const tTablero incognita, tTablero tablero, tElemento *elemento, tEstado * juego);
static int unflag(const tTablero incognita, tTablero tablero, tElemento * elemento, tEstado * eJuego);
static int barrerCasillero(const tTablero incognita, tTablero tablero, tElemento * elemento, tEstado * eJuego);
static int query(const tTablero incognita, int filas, int columnas, tElemento * ini, tRunCmd * toRun);

tTablero generarTablero(int filas, int columnas) {
	int i;
	tTablero tablero;

	tablero = malloc(filas * sizeof(tFila));
	if(tablero == NULL)
		return NULL;
	for( i = 0; i < filas; i++) {
		tablero[i] = malloc(columnas * sizeof(tTablero));
		if(tablero[i] == NULL) {
			freeTablero(tablero, i);
			return NULL;
		}
	}
	return tablero;
}

int setDificultad(tParametros *parametros, int casilleros) {
	switch(parametros->nivel) {
		case 1: 
			parametros->undos = U_FACIL;
			parametros->minas = casilleros * PORCENTAJE(P_FACIL);
			break;
		case 2: 
			parametros->undos = U_MEDIO;
			parametros->minas = casilleros * PORCENTAJE(P_MEDIO);
			break;
		case 3: 
			parametros->undos = U_DIFICIL;
			parametros->minas = casilleros * PORCENTAJE(P_DIFICIL);
			break;
		case 4:
			if(casilleros < 100)
				return ERROR_CASILLEROS;
			parametros->undos = U_PESADILLA;
			parametros->minas = casilleros * PORCENTAJE(P_PESADILLA);
			break;
		default:
			return ERROR;
	}
	return OK;
}

int guardarDatos(const tTablero incognita, const tTablero tablero, int filas, int columnas, tParametros * p, const tArchivo fileSave) {
	int i;
	FILE *fp;

	fp = fopen(fileSave, "wb");
	if(fp == NULL)
		return ERROR_NAME_FILE;

	fwrite(&(p->nivel), sizeof(p->nivel), 1, fp);

	fwrite(&filas, sizeof(filas), 1, fp);
	fwrite(&columnas, sizeof(columnas), 1, fp);

	fwrite(&(p->undos), sizeof(p->undos), 1, fp);
	if(p->movimientos < 0)
		p->movimientos = INF;
	fwrite(&(p->movimientos), sizeof(p->movimientos), 1, fp);
	fwrite(&(p->campania), sizeof(p->campania), 1, fp);

	for(i = 0; i < filas; i++)
		fwrite(incognita[i], sizeof(**incognita), columnas, fp);
	for(i = 0; i < filas; i++)
		fwrite(tablero[i], sizeof(**tablero), columnas, fp);

	if(p->campania)
		fwrite(&(p->fileCamp), sizeof(p->fileCamp[0]), MAX_FILE_NAME, fp);

	fclose(fp);

	return OK;
}

int cargarDatos(tTablero * incognita, tTablero * tablero, int * filas, int * columnas, tParametros * p, tArchivo saveFile) {
	int i, leidos;
	FILE * fp;

	fp = fopen(saveFile, "rb");
	if(fp == NULL)
		return ERROR_OPEN_FILE;

	leidos = fread(&(p->nivel), sizeof(p->nivel), 1, fp);
	if(leidos != 1)
		return ERROR_CORRUPT_FILE;

	leidos = fread(filas, sizeof(*filas), 1, fp);
	if(leidos != 1 || *filas < MIN_FIL || *filas > MAX_FIL)
		return ERROR_CORRUPT_FILE;

	leidos = fread(columnas, sizeof(*columnas), 1, fp);
	if(leidos != 1 || *columnas < MIN_COL || *columnas > MAX_COL)
		return ERROR_CORRUPT_FILE;

	if(!setDificultad(p, (*filas) * (*columnas)))
		return ERROR_CORRUPT_FILE;

	leidos = fread(&(p->undos), sizeof(p->undos), 1, fp);
	if(leidos != 1 || p->undos < 0)
		return ERROR_CORRUPT_FILE;

	leidos = fread(&(p->movimientos), sizeof(p->movimientos), 1, fp);
	if(leidos != 1 || p->movimientos < 0)
		return ERROR_CORRUPT_FILE;

	leidos = fread(&(p->campania), sizeof(p->campania), 1, fp);
	if(leidos != 1)
		return ERROR_CORRUPT_FILE;

	*incognita = generarTablero(*filas, *columnas);
	if(*incognita == NULL)
		return ERROR_MALLOC;
	*tablero = generarTablero(*filas, *columnas);
	if(*tablero == NULL) {
		freeTablero(*incognita, *filas);
		return ERROR_MALLOC;
	}

	for(i = 0; i < *filas; i++) {
		leidos = fread((*incognita)[i], sizeof(***incognita), *columnas, fp);
		if(leidos != *columnas)
			return ERROR_CORRUPT_FILE;
	}
	for(i = 0; i < *filas; i++) {
		leidos = fread((*tablero)[i], sizeof(***tablero), *columnas, fp);
		if(leidos != *columnas)
			return ERROR_CORRUPT_FILE;
	}

	if(p->campania) {
		leidos = fread(&(p->fileCamp), sizeof(p->fileCamp[0]), MAX_FILE_NAME, fp);
		if(leidos != MAX_FILE_NAME)
			return ERROR_CORRUPT_FILE;
	}

	fclose(fp);

	return OK;
}

int prepararJuego(tTablero *incognita, tTablero *tablero, int filas, int columnas, tParametros *parametros) {
	*incognita = generarTablero(filas, columnas);
	if(*incognita == NULL)
		return ERROR_MALLOC;
	*tablero = generarTablero(filas, columnas);
	if(*tablero == NULL) {
		freeTablero(*tablero, filas);
		return ERROR_MALLOC;
	}

	setDificultad(parametros, (filas * columnas));

	llenarTablero(*tablero, filas, columnas, NO_TOCADO);
	llenarTablero(*incognita, filas, columnas, VACIO);
	llenarMinas(*incognita, filas, columnas, parametros);

	return OK;
}

void llenarTablero(tTablero tablero, int filas, int columnas, char obj)  {
	int i;

	for(i = 0; i < filas; i++) 
		memset(tablero[i], obj, columnas);
}

void llenarMinas(tTablero tablero, int filas, int columnas, tParametros *parametros) {
	int i;
	int randFil, randCol;

	for( i = 0; i < parametros -> minas; ) {
		randFil = randInt(0, filas - 1);
		randCol = randInt(0, columnas - 1);
		if(tablero[randFil][randCol] != MINA) {
			tablero[randFil][randCol] = MINA;
			i++;
		}
	}
}

void analizarTablero(const tTablero incognita, const tTablero tablero, int filas, int columnas, tEstado * eJuego) {
	int i, j;

	for(i = 0; i < filas; i++)
		for(j = 0; j < columnas; j++) {
			if(tablero[i][j] == FLAG) {
				eJuego->flagsUsados++;
				if(incognita[i][j] == MINA)
					eJuego->flagsCorrectos++;
			} 
			else if(tablero[i][j] == VACIO)
				eJuego->sCorrectos++;
		}
}

int validaCmd(const tTablero incognita, tTablero tablero, int filas, int columnas, tParametros * p, char * comando, tRunCmd * toRun) {
	char bracket, specialC, nullChar, fila, unChar;
	/*nullChar verifica que no haya más caracteres luego del fin de un comando*/
	tElemento ini, fin;
	int columna;

	switch(*comando) {
		case 'f':
			// Caso flag (x,y)
			if(sscanf(comando, "flag (%c,%d%c%c", &ini.fil, &ini.col, &bracket, &nullChar) == 3) {
				if(bracket == ')' && isalpha(ini.fil)) {
					if(posValida(&ini, filas, columnas)) {
						if(tablero[ini.fil - 'A'][ini.col] == FLAG)
							return ERROR_CMD_CANTDO;
						toRun->cmd = FLAG;
						toRun->ini.fil = ini.fil;
						toRun->ini.col = ini.col;
						toRun->fin.fil = ini.fil;
						toRun->fin.col = ini.col;
						return OK;
					}
					else 
						return ERROR_CMD_RANGE;
				}
			}
			// Caso flag (x,y:z,w)
			else if(sscanf(comando, "flag (%c,%d:%c,%d%c%c", &ini.fil, &ini.col, &fin.fil, &fin.col, &bracket, &nullChar) == 5)
				if(bracket == ')' && isalpha(ini.fil) && isalpha(fin.fil)) {
					if(rangoValido(&ini, &fin, filas, columnas)) {
						toRun->cmd = FLAG;
						toRun->ini.fil = ini.fil;
						toRun->ini.col = ini.col;
						toRun->fin.fil = fin.fil;
						toRun->fin.col = fin.col;
						return OK;
					} else
						return ERROR_CMD_RANGE;
				}
			break;

		case 'q':
			// Caso quit
			if(!strcmp(comando, "quit")) {
				toRun->cmd = QUIT;
				return OK;
			}
			// Caso query C
			else if (sscanf(comando,"query%c%d%c", &specialC,&columna,&nullChar) == 2 && specialC==' ') {
				columna--;
				if(columna >= 0 && columna < columnas && specialC==' ') {
					toRun->cmd = QUERY;
					toRun->ini.fil = 0;
					toRun->ini.col = columna;	
					return QUERY;
				} else
					return ERROR_CMD_RANGE;	
			} 
			// Caso query F
			else if (sscanf(comando,"query%c%c%c", &specialC, &fila, &nullChar) == 2 && specialC==' ') {
				if(isalpha(fila)) {
					fila = toupper(fila);
					if(fila - 'A' >= 0 && fila - 'A' < filas) {
						toRun->cmd = QUERY;
						toRun->ini.fil = fila;
						toRun->ini.col = 0;	
						return OK;
					} else
						return ERROR_CMD_RANGE;
				}
			}
			break;

		case 's':
		case 'S':
			// Caso save filename
			if(sscanf(comando, "save %c", &specialC) && !iscntrl(specialC)) {
				toRun->cmd = SAVE;
				toRun->saveFile = comando + 5;
				return SAVE;
			}
			// Caso s
			else if(sscanf(comando, "%[sS] (%c,%d%c%c", &unChar, &ini.fil, &ini.col, &bracket, &nullChar) == 4)
				if(bracket == ')' && isalpha(ini.fil)) {
					if(posValida(&ini, filas, columnas)) {
						toRun->cmd = S;
						toRun->ini.fil = ini.fil;
						toRun->ini.col = ini.col;
						return OK;
					} else
						return ERROR_CMD_RANGE;
				}
			break;

		case 'u':
			// Caso undo
			if(!strcmp(comando, "undo")) {
				if(p->undos <= 0)
					return ERROR_UNDOS;
				toRun->cmd = UNDO;
				return OK;
			}
			// Caso unflag(x,y)
			else if(sscanf(comando, "unflag (%c,%d%c%c", &ini.fil, &ini.col, &bracket, &nullChar) == 3) {
				if(bracket == ')' && isalpha(ini.fil)) {
					if(posValida(&ini, filas, columnas)) {
						toRun->cmd = UNFLAG;
						toRun->ini.fil = ini.fil;
						toRun->ini.col = ini.col;
						toRun->fin.fil = ini.fil;
						toRun->fin.col = ini.col;
						return OK;
					}
					return ERROR_CMD_RANGE;
				}
			}
			// Caso unflag(x,y:z,w)
			else if(sscanf(comando, "unflag (%c,%d:%c,%d%c%c", &ini.fil, &ini.col, &fin.fil, &fin.col, &bracket, &nullChar) == 5)
				if(bracket == ')' && isalpha(ini.fil) && isalpha(fin.fil)) {
					if(rangoValido(&ini, &fin, filas, columnas)) {
						toRun->cmd = UNFLAG;
						toRun->ini.fil = ini.fil;
						toRun->ini.col = ini.col;
						toRun->fin.fil = fin.fil;
						toRun->fin.col = fin.col;
						return OK;
					}
					else
						return ERROR_CMD_RANGE;
				}
			break;
	}
	return ERROR_CMD_NAME;
}

int ejecutaCmd(const tTablero incognita, tTablero tablero, int filas, int columnas, tParametros * p, tEstado * eJuego, tComando * lastCmd, tRunCmd * toRun) {
	int cant, doFil = FALSE, ret;

	switch(toRun->cmd) {
		case UNFLAG:
		case FLAG:
			cant = toRun->fin.fil - toRun->ini.fil + 1;
			if(cant == 1) {
				doFil = TRUE;
				cant = toRun->fin.col - toRun->ini.col + 1;
			}		

			if(toRun->cmd == FLAG && p->minas - eJuego->flagsUsados - cant < 0)
				return ERROR_FLAGS;

			if(!eJuego->infMoves && (p->movimientos - cant) < 0)
				return ERROR_MOVS;

			while(cant--) {
				if(toRun->cmd == FLAG) {
					ret = flag(incognita, tablero, &(toRun->ini), eJuego);
					if(ret)
						lastCmd->cmd = FLAG;
				}
				else {
					ret = unflag(incognita, tablero, &(toRun->ini), eJuego);
					if(ret)
						lastCmd->cmd = UNFLAG;
				}
				if(ret) {
					p->movimientos--;
					lastCmd->pos.fil = toRun->ini.fil;
					lastCmd->pos.col = toRun->ini.col;
				}
				if(doFil)
					toRun->ini.col++;
				else
					toRun->ini.fil++; 
			}
			return OK;

		case SAVE:
			ret = guardarDatos(incognita, tablero, filas, columnas, p, toRun->saveFile);
			if(ret < OK)
				return ret;
			return SAVE;

		case S:
			ret = barrerCasillero(incognita, tablero, &(toRun->ini), eJuego);
			if(ret < OK )
				return ret;
			p->movimientos--;
			lastCmd->cmd = S;
			lastCmd->pos.fil = toRun->ini.fil;
			lastCmd->pos.col = toRun->ini.col;
			return S;

		case UNDO:
			if(lastCmd->cmd == FLAG || lastCmd->cmd == UNFLAG) {
				toRun->ini.fil = lastCmd->pos.fil;
				toRun->ini.col = lastCmd->pos.col;
				if(lastCmd->cmd == FLAG)
					unflag(incognita, tablero, &(toRun->ini), eJuego);
				else
					flag(incognita, tablero, &(toRun->ini), eJuego);
				p->movimientos--;
				lastCmd->cmd = UNDO;
				p->undos--;
				return OK;
			}
			if(lastCmd->cmd == S) {
				tablero[lastCmd->pos.fil - 'A'][lastCmd->pos.col] = NO_TOCADO;
				p->movimientos--;
				eJuego->sCorrectos--;
				p->undos--;
				eJuego->barrioMina = FALSE;
				lastCmd->cmd = UNDO;
				return OK;
			}
			return ERROR_CMD_CANTDO;

		case QUERY:
			ret = query(incognita, filas, columnas, &(toRun->ini), toRun);
			if(ret < OK)
				return ERROR_MALLOC;
			return QUERY;

		case QUIT:
			return QUIT;
	}
	return ERROR;
}

int posValida(tElemento * elemento, int filas, int columnas) {
	elemento->fil = toupper(elemento->fil);
	elemento->col--;
	return (elemento->fil - 'A') >= 0 && elemento->col >= 0 && (elemento->fil - 'A') < filas && elemento->col < columnas;
}

int rangoValido(tElemento * inicial, tElemento * final, int filas, int columnas) {
	char auxF;
	int auxC;

	if(!posValida(inicial, filas, columnas) || !posValida(final, filas, columnas))
		return ERROR;

	if(inicial->fil != final->fil && inicial->col != final->col)
		return ERROR;
	if(inicial->fil > final->fil)
		SWAP(inicial->fil, final->fil, auxF);
	if(inicial->col > final->col)
		SWAP(inicial->col, final->col, auxC);

	return OK;
}

int flag(const tTablero incognita, tTablero tablero, tElemento * elemento, tEstado * eJuego) {
	int i = elemento->fil - 'A';
	int j = elemento->col;

	if(tablero[i][j] == NO_TOCADO) {
		tablero[i][j] = FLAG;
		eJuego->flagsUsados++;
		if(incognita[i][j] == MINA)
			eJuego->flagsCorrectos++;
		return OK;
	}

	return ERROR;
}

int unflag(const tTablero incognita, tTablero tablero, tElemento * elemento, tEstado * eJuego) {
	int i = elemento->fil - 'A';
	int j = elemento->col;

	if(tablero[i][j] == FLAG) {
		tablero[i][j] = NO_TOCADO;
		eJuego->flagsUsados--;
		if(incognita[i][j] == MINA)
			eJuego->flagsCorrectos--;
		return OK;
	}

	return ERROR;
}

int barrerCasillero(const tTablero incognita, tTablero tablero, tElemento * elemento, tEstado * eJuego) { 
	int i = elemento->fil - 'A';
	int j = elemento->col;

	if(tablero[i][j] == VACIO)
		return ERROR_S_VACIO;

	if(tablero[i][j] == FLAG)
		return ERROR_S_FLAG;

	tablero[i][j] = VACIO;
	if(incognita[i][j] == MINA) {
		tablero[i][j] = MINA;
		eJuego->barrioMina = TRUE;
	}

	return OK;
}

int query(const tTablero incognita, int filas, int columnas, tElemento * ini, tRunCmd * toRun) {
	int i = ini->fil - 'A', j = 0, dim = 0;
	int * positions = NULL, * aux;
	char habiaMina = FALSE;

	if(ini->fil == 0) {
		j = ini->col ;
		i = 0;
	}

	while(i < filas && j < columnas) {
		if(dim % BLOQUE == 0) {
			aux = realloc(positions, (dim + BLOQUE) * sizeof(*positions));
			if (aux == NULL) {
				free(positions);
				return ERROR;
			}
			positions = aux;
		}
		if(incognita[i][j] == MINA) {	
			if(!habiaMina) { //Comienza a sumar minas que están juntas sobre una fila o columna
				habiaMina = TRUE;
				positions[dim] = 0;
			}
			positions[dim]++;
		}
		else if(habiaMina) { /*Si no encontró una mina pero previamente hubo*/
			habiaMina = FALSE;
			dim++;
		}
		if(ini->fil == 0) /*Preguntar si hay que iterar sobre una fila o una columna*/
			i++;
		else
			j++; 
	}
	if(habiaMina) /*En caso de que haya encontrado una mina al final del tablero*/
		dim++;

	aux = realloc(positions, (dim + 1) * sizeof(*positions));
	if(aux == NULL) {
		free(positions);
		return ERROR;
	}
	positions = aux;

	positions[dim] = EOA;
	
	toRun->queryVec = positions;
	return OK;
}

char * leerString(void) {
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
	if(dim > 0)
		string[dim] = '\0';
	return string;
}

void freeTablero(tTablero tablero, int filas) {
	int i;

	for (i = 0; i < filas; i++)
		free(tablero[i]);
	free(tablero);
}

double randNormalize(void) {
	return rand() / ((double) RAND_MAX + 1);
}

int randInt(int izq, int der) {
	return	(randNormalize() * (der - izq + 1) + izq);
}