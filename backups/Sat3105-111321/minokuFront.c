#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "getnum.h"
#include "minokuBack.h"
#include "misc.h"

// ~~~ CONDICIONES ~~~

// RESULTADO // Usar enum
#define GANO 2
#define SIGUE 1
#define PERDIO 0

// JUEGO
//
int jugar(const TipoTablero incognita, TipoTablero tablero, int filas, int columnas, TipoParametros * parametros, char * fileCamp);
//
int juegoNuevo(void);
//
int cargarJuego(void);
//
int jugarCampania(TipoTablero incognita, TipoTablero tablero, int filas, int columnas, TipoParametros *parametros, FILE * fp, int nivel);

// PRINTEO
// 
void imprimirEstado(TipoTablero tablero, int filas, int columnas, TipoParametros *parametros);
// 
void imprimirTablero(TipoTablero tablero, int filas, int columnas);

// SETEO & PREPARACION
//
void setDimTablero(int * filas, int * columnas);
//
int leerDificultad(int casilleros);

int main(void) 
{
	int opcion, valido;

	srand(time(NULL));
	
	printf("-------------------\n");
	printf("Bienvenido a Minoku\n");
	printf("-------------------\n\n");
	do
	{
		valido = TRUE;
		printf("1) Juego Individual\n2) Cargar Juego\n3) Salir\n");
		opcion = getint("Opcion: ");
		switch(opcion)
		{
			case 1:	
				juegoNuevo();
				break;
			case 2:	
				cargarJuego();
				break;
			case 3:	
				return 0;
			default:
				valido = FALSE;
				break;
		}
	}
	while(!valido);

	return 0;
}

int juegoNuevo(void)
{
	TipoTablero incognita = NULL, tablero = NULL;
	TipoParametros parametros;
	int filas, columnas;
	int opcion, valido, ret;
	char *archivo;
	int nivel = 1;
	FILE *fp;

	parametros.movimientos = INF;

	do
	{
		valido = TRUE;
		printa("Juego Nuevo");
		printf("1) Juego individual sin límite de movimientos\n");
		printf("2) Juego individual con límite de movimientos\n");
		printf("3) Juego por campaña\n");
		opcion = getint("Opcion: ");
		switch(opcion)
		{
			case 1:
			case 2:
				parametros.campania = FALSE; // Setea que no se va a jugar una campania

				setDimTablero(&filas, &columnas); // Lee las filas y columnas
				parametros.dificultad = leerDificultad(filas * columnas); // Lee la dificultad

				ret = prepararJuego(&incognita, &tablero, filas, columnas, &parametros); // Prepara todo para jugar
				if(ret != OK) {
					printError(ret); // En caso de error lo imprime y sale
					return ERROR;
				}

				if(opcion == 2)
					parametros.movimientos = parametros.cantUndo + parametros.cantMinas; // Si es con limite lo setea

				jugar(incognita, tablero, filas, columnas, &parametros); // Comienza el juego

				break;
			case 3:	
				parametros.campania = TRUE; // Juego campaña

				do {
					printf("Introducir el nombre del archivo campania:\n");
					archivo = leerString(); // Lee el archivo campaña
					if (archivo != NULL)
					{
						fp = fopen(archivo, "r");
						if(fp == NULL) 
							printError(ERROR_OPEN_FILE); // En caso de error: lo imprime
					} else 
						printError(ERROR_MALLOC);
				} while(fp == NULL || archivo == NULL);

				jugarCampania(incognita, tablero, filas, columnas, &parametros, fp, nivel); // Comienza la campaña

				break;
			default:	
				valido = FALSE;

				break;
		}
	}
	while (!valido);

	return OK;
}

int cargarJuego(void) 
{
	TipoTablero incognita = NULL, tablero = NULL;
	int filas, columnas;
	int nivel = 0, ret, resultado, error;
	TipoParametros parametros;
	int auxFilas, auxColumnas, auxDificultad;
	char * fileSaved;
	char * fileCamp = NULL;
	FILE * fp;

	do { // Lee el archivo guardado, en caso de error: lo imprime
		error = FALSE;
		printf("Introducir el nombre del archivo guardado:\n");
		fileSaved = leerString(); 
		if(fileSaved != NULL)
		{
			ret = cargarDatos(&incognita, &tablero, &filas, &columnas, &parametros, fileCamp, fileSaved);
			if(ret != OK) 
			{
				printError(ret);
				if(ret != ERROR_OPEN_FILE)
					return ERROR;
				error = TRUE;
			}
		} else {
			printError(ERROR_MALLOC);
			return ERROR;
		}
	} while(fileSaved == NULL || error);

	if(parametros.campania)
	{
		fp = fopen(fileCamp, "r"); // Si es campaña, entonces abre el archivo de esa campaña
		if(fp == NULL) {
			printError(ERROR_OPEN_FILE); // En caso de que el archivo no exista tira error
			return ERROR;
		}

		do { // Avanza el archivo hasta el nivel en el que se encontraba cuando guardo
			fscanf(fp, "%d\t%dx%d", &auxDificultad, &auxFilas, &auxColumnas);
			nivel++;
		} while(parametros.dificultad != auxDificultad || filas != auxFilas || columnas != auxColumnas);

		// Juega el nivel guardado
		printf("Nivel: %d\n", nivel++);
		printf("Tablero de %dx%d: \n", filas, columnas);
		printf("Dificultad: %d\n", parametros.dificultad);
	}

	resultado = jugar(incognita, tablero, filas, columnas, &parametros);

	if(parametros.campania && resultado == GANO) // Sigue jugando el resto de la campaña
		jugarCampania(incognita, tablero, filas, columnas, &parametros, fp, nivel);

	return OK;
}

// Asume que el archivo ya esta abierto
int jugarCampania(TipoTablero incognita, TipoTablero tablero, int filas, int columnas, TipoParametros *parametros, FILE * fp, int nivel) 
{
	int ret = GANO;

	while(!feof(fp) && ret == GANO) {
		fscanf(fp, "%d\t%dx%d", &(parametros->dificultad), &filas, &columnas);

		ret = prepararJuego(&incognita, &tablero, filas, columnas, parametros);
		if(ret != OK) {
			printError(ret);
			return ERROR;
		}

		parametros->movimientos = parametros->cantUndo + parametros->cantMinas;

		printf("Nivel: %d\n", nivel++);
		printf("Tablero de %dx%d: \n", filas, columnas);
		printf("Dificultad: %d\n", parametros->dificultad);
		ret = jugar(incognita, tablero, filas, columnas, parametros);
	}
	fclose(fp);

	return OK;
}

int jugar(const TipoTablero incognita, TipoTablero tablero, int filas, int columnas, TipoParametros * parametros, char * fileCamp)
{
	tEstadoJuego = { 0, 0, 0, FALSE};
	int ret = SEGUIR;
	char * funcion, * parametros;
	TipoUndo preventivo;

	while(ret == SEGUIR) 
	{
		imprimirEstado(tablero, filas, columnas, parametros);
		preventivo = copiadoPreventivo(tablero, filas, columnas, parametros->flags, flagsCorrectos, sCorrectos);
		do
			scanf("%s %s", funcion, parametros);
		while(ret = validarComando(funcion, parametros));
		correrComando();
		ret = seguirJuego(parametros, filas*columnas, flagsCorrectos, sCorrectos)
	}
	imprimirTablero(incognita, filas, columnas);

	return ret;
}

int seguirJuego(TipoTablero * parametros, int casilleros, tEstadoJuego juego) 
{
	int hacer;

	if(barrioMina) 
	{
		printa("HAS PERDIDO");
		if(parametros->cantUndo > 0 && parametros->movimientos > 1) 
		{
			printf("Pero todavia dispones de %d undos.\n", parametros->cantUndo);
			hacer = yesNo("Deseas utilizar uno ahora? Caso contrario pierdes...");
			if(hacer) 
			{
				doUndo();
				return SIGUE;
			}
		}
		return PERDIO;
	}

	if(parametros->movimientos == 0) 
	{
		printa("HAS PERDIO");
		return PERDIO;
	} //else FALTA TERMINAR

	if(sCorrectos == casilleros - parametros->cantMinas) 
	{
		printa("HAS GANADO");
		return GANO;
	}

	if(flagsCorrectos == parametros->cantMinas) 
	{
		printa("HAS GANADO");
		return GANO;
	}

	return SIGUE;
}

void setDimTablero(int * filas, int * columnas)
{
	do
		*filas = getint("Inserte la cantidad de filas (Min. = %d)(Max. = %d): ", MIN_FIL, MAX_COL);
	while (*filas < MIN_FIL || *filas > MAX_FIL);
	do
		*columnas = getint("Inserte la cantidad de columnas (Min. = %d)(Max. = %d): ", MIN_COL, MAX_COL);
	while (*columnas < MIN_COL || *columnas > MAX_COL);
}

int leerDificultad(int casilleros) 
{
	int nivel, valido;

	do {
		valido = TRUE;
		printf("Ingrese la dificultad:\n");
		printf("1) Facil\n");
		printf("2) Medio\n");
		printf("3) Dificil\n");
		printf("4) Pesadilla (requiere minimo de 100 casilleros)\n");
		nivel = getint("Ingrese la opcion: ");
		if(nivel == 4 && casilleros < 100) {
			printa("Este nivel requiere un minimo de 100 casilleros");
			valido = FALSE;
		}
	} while(!valido || nivel <= 0 || nivel > 4);
	return nivel;
}

void imprimirEstado(TipoTablero tablero, int filas, int columnas, TipoParametros *parametros)
{
	if(parametros -> movimientos != INF)	
		printf("Cantidad de movimientos restantes: %d\n", parametros -> movimientos);
	printf("Cantidad de flags restantes: %d\n", parametros -> flags);
	printf("Cantidad de undos restantes: %d\n", parametros -> cantUndo);
	imprimirTablero(tablero, filas, columnas);
}

void imprimirTablero(TipoTablero tablero, int filas, int columnas)
{
	int i, j, num;
	char letra = 'A';

	putchar('\t');
	for ( num = 1; num <= columnas; num++)
		printf("%d\t", num);
	putchar('\n');
	
	for ( i = 0; i < filas; i++)
	{
		printf("%c\t", letra++);
		for ( j = 0; j < columnas; j++ )
			printf("%c\t", tablero[i][j]);
		putchar('\n');
	}
}
