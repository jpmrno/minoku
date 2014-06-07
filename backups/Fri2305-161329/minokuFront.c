#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "getnum.h"
#include "minokuBack.h"
#include "misc.h"

// ~~~ CONDICIONES ~~~
// Condiciones para el tablero
#define MIN_FIL 5
#define MIN_COL 5
#define MAX_FIL 19
#define MAX_COL 19
#define BLOQUE 5
#define INF 0

// JUEGO
//
void jugar(const TipoTablero incognita, TipoTablero tablero, int filas, int columnas, TipoParametros * parametros);
//
int juegoNuevo(void);
//
int cargarJuego(void);

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
	int opcion, valido;
	char archivo[20];
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
				setDificultad(&parametros, (filas * columnas));

				if(!prepararTableros(&incognita, &tablero, filas, columnas, &parametros)) {
					printa("No se pudo inicializar el juego");
					return ERROR;
				}

				jugar(incognita, tablero, filas, columnas, &parametros);
				break;
			case 3:	
				parametros.campania = TRUE;

				do {
					printf("Introducir el nombre del archivo campania:\n");
					scanf("%19s", archivo);
					fp = fopen(archivo, "r");
					if(fp == NULL) 
						printa("No se puede abrir el archivo. Intenta otra vez");
				} while(fp == NULL);

				do {
					fscanf(fp, "%d\t%dx%d", &parametros.dificultad, &filas, &columnas);

					setDificultad(&parametros, (filas * columnas));

					if(!prepararTableros(&incognita, &tablero, filas, columnas, &parametros)) {
						printa("No se pudo inicializar el juego");
						return ERROR;
					}

					parametros.movimientos = (parametros.cantUndo + parametros.cantMinas);

					printf("Nivel: %d\n", nivel++);
					printf("Tablero de %dx%d: \n", filas, columnas);
					printf("Dificultad: %d\n", parametros.dificultad);
					jugar(incognita, tablero, filas, columnas, &parametros);
				} while(!feof(fp));
				fclose(fp);
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
	int auxFilas, auxColumnas, auxDificultad;
	TipoParametros parametros;
	char archivo[20];
	int nivel = 1;
	FILE *fp;

	do {
		printf("Introducir el nombre del archivo guardado:\n");
		scanf("%19s", archivo);
		fp = fopen(archivo, "r");
		if(fp == NULL) 
			printa("No se puede abrir el archivo. Intenta otra vez");
	} while(fp == NULL);

	cargarDatos(fp, &incognita, &tablero, &filas, &columnas, &parametros);
	fclose(fp);

	if(parametros.campania)
	{
		do {
			printf("Introducir el nombre del archivo campania:\n");
			scanf("%19s", archivo);
			fp = fopen(archivo, "r");
			if(fp == NULL) 
				printa("No se puede abrir el archivo. Intenta otra vez");
		} while(fp == NULL);

		do {
			nivel++;
			fscanf(fp, "%d\t%dx%d", &auxDificultad, &auxFilas, &auxColumnas);
		} while(parametros.dificultad != auxDificultad || filas != auxFilas || columnas != auxColumnas);

		do {
			fscanf(fp, "%d\t%dx%d", &parametros.dificultad, &filas, &columnas);

			setDificultad(&parametros, (filas * columnas));

			if(!prepararTableros(&incognita, &tablero, filas, columnas, &parametros)) {
				printa("No se pudo inicializar el juego");
				return ERROR;
			}

			parametros.movimientos = (parametros.cantUndo + parametros.cantMinas);

			printf("Nivel: %d\n", nivel++);
			printf("Tablero de %dx%d: \n", filas, columnas);
			printf("Dificultad: %d\n", parametros.dificultad);
			jugar(incognita, tablero, filas, columnas, &parametros);
		} while(!feof(fp));
		fclose(fp);
	} 
	else 
		jugar(incognita, tablero, filas, columnas, &parametros);
	return OK;
}

void jugar(const TipoTablero incognita, TipoTablero tablero, int filas, int columnas, TipoParametros *parametros)
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
	int opcion, valido;

	do {
		valido = TRUE;
		printf("Ingrese la dificultad:\n");
		printf("1) Facil\n");
		printf("2) Medio\n");
		printf("3) Dificil\n");
		printf("4) Pesadilla (requiere minimo de 100 casilleros)\n");
		opcion = getint("Ingrese la opcion: ");
		if(opcion == 4 && casilleros < 100) {
			printa("Este nivel requiere un minimo de 100 casilleros");
			valido = FALSE;
		}
	} while(!valido || opcion <= 0 || opcion > 4);
	return opcion;
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
