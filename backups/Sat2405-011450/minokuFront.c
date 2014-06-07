#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "getnum.h"
#include "minokuBack.h"
#include "misc.h"

// ~~~ CONDICIONES ~~~

// RESULTADO
#define GANO 1
#define PERDIO 0

// JUEGO
//
int jugar(const TipoTablero incognita, TipoTablero tablero, int filas, int columnas, TipoParametros * parametros);
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
			case 1:	juegoNuevo();
					break;
			case 2:	cargarJuego();
					break;
			case 3:	return 0;
			default:valido = FALSE;
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
			case 2:	
				parametros.movimientos = parametros.cantUndo + parametros.cantMinas;
			case 1:
				parametros.campania = FALSE;

				setDimTablero(&filas, &columnas);
				parametros.dificultad = leerDificultad(filas * columnas);

				ret = prepararJuego(&incognita, &tablero, filas, columnas, &parametros);
				if(ret != OK) {
					printError(ret);
					return ERROR;
				}

				jugar(incognita, tablero, filas, columnas, &parametros);

				break;
			case 3:	
				parametros.campania = TRUE;

				do {
					printf("Introducir el nombre del archivo campania:\n");
					archivo = leerString();
					if (archivo != NULL)
					{
						fp = fopen(archivo, "r");
						if(fp == NULL) 
							printError(ERROR_OPEN_FILE);
					} else 
						printError(ERROR_MALLOC);
				} while(fp == NULL || archivo == NULL);

				jugarCampania(incognita, tablero, filas, columnas, &parametros, fp, nivel);

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
	int auxFilas, auxColumnas, auxDificultad; //ES PARA SABER NUM. DE LINEA DE CAMPAÑA GUARDADA
	char * fileSaved;
	char * fileCamp = NULL;
	FILE * fp;

	do { // Lee el nombre del archivo del juego guardado
		printf("Introducir el nombre del archivo guardado:\n");
		fileSaved = leerString();
		printf("%s\n", fileSaved);
		if(fileSaved != NULL)
		{
			ret = cargarDatos(&incognita, &tablero, &filas, &columnas, &parametros, fileSaved, fileCamp);
			if(ret != OK) 
			{
				printError(ret);
				if(ret != ERROR_OPEN_FILE)
					return ERROR;
				error = TRUE;
			}
		} else
			printError(ERROR_MALLOC);
	} while(fileSaved == NULL);

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

	if(parametros.campania && resultado == GANO) // Termina de jugar el resto de la campaña
		jugarCampania(incognita, tablero, filas, columnas, &parametros, fp, nivel);

	return OK;
}

int jugarCampania(TipoTablero incognita, TipoTablero tablero, int filas, int columnas, TipoParametros *parametros, FILE * fp, int nivel) 
{
	int ret;

	do {
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
		jugar(incognita, tablero, filas, columnas, parametros);
	} while(!feof(fp));
	fclose(fp);

	return OK;
}

int jugar(const TipoTablero incognita, TipoTablero tablero, int filas, int columnas, TipoParametros *parametros)
{
	//char * comando;
	//TipoTablero auxTablero;
	FILE *fp;
	//while(parametros -> movimientos != 0) {
		//imprimirEstado(tablero, filas, columnas, parametros);
		//auxTablero = copiaTablero(tablero, filas, columnas);
	/*if(filas == 5 && columnas == 7) {
		fp = fopen("guardado2.bin","w");
		guardarDatos(fp, incognita, tablero, filas, columnas, parametros);
		fclose(fp);
	}*/
		//do
			//comando=leerComando();
			//realizarComando();
		//while(query);
	//}
	imprimirTablero(incognita, filas, columnas);
	imprimirEstado(tablero, filas, columnas, parametros);
	return OK;
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
