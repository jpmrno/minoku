#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "getnum.h"
#include "minokuBack.h"
#include "misc.h"

#define PORCENTAJE(x) ((double) (x)/100)

// CONDICIONES (conviene mandarlo a otro .h?)
// Condiciones para el tablero
#define MIN_FIL 5
#define MIN_COL 5
#define MAX_FIL 19
#define MAX_COL 19
// Condiciones para la dificultad
#define P_FACIL 20
#define P_MEDIO 50
#define P_DIFICIL 70
#define P_PESADILLA 90
#define U_FACIL 10
#define U_MEDIO 5
#define U_DIFICIL 3
#define U_PESADILLA 1

// Iniciales
void setDimTablero(int * filas, int * columnas);
int setDificultad(int * cantUndo, int * cantMinas, int casilleros);

int jugar(const TipoTablero incognita, int filas, int columnas, int cantMinas, int cantUndo);

void imprimirTablero(TipoTablero tablero, int filas, int columnas);

int main(void) 
{
	int filas, columnas;
	int cantMinas, cantUndo;
	TipoTablero incognita;
	
	srand(time(NULL));

	setDimTablero(&filas, &columnas);
	incognita = generarTablero(filas, columnas);
	if(incognita == NULL)
	{
		return ERROR;
	}
	setDificultad(&cantUndo, &cantMinas, filas * columnas);
	llenarTablero(incognita, filas, columnas, cantMinas);

	// Jugar no necesitaria demaciados parametros?
	// Conviene pasar algunas creaciones al jugar?
	// Tambien le deberia pasar lim de movimientos
	if(jugar(incognita, filas, columnas, cantMinas, cantUndo)) {
		printa("Juego terminado con exito.");
		
		// Cuando termina el programa tengo que liberar las matrices.
		liberarMatriz(incognita, filas);
		return 0;
	}
	printa("Hubo un error en el juego.");
	return 1;
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

void setDimTablero(int * filas, int * columnas)
{
	do
		*filas = getint("Inserte la cantidad de filas (Min. = %d): ", MIN_FIL);
	while (*filas < MIN_FIL || *filas > MAX_FIL);
	do
		*columnas = getint("Inserte la cantidad de filas (Min. = %d): ", MIN_COL);
	while (*columnas < MIN_COL || *columnas > MAX_COL);
}

int setDificultad(int * cantUndo, int * cantMinas, int casilleros)
{
	char opcion, valido;

	do
	{
		printf("Ingrese una de las siguientes dificultades:\n");
		printf("1 - Facil\n");
		printf("2 - Medio\n");
		printf("3 - Dificil\n");
		printf("4 - Pesadilla\n");
		opcion = getint("Ingrese la opcion: ");
		valido = TRUE;
		switch(opcion)
		{
				case 1: *cantUndo = U_FACIL;
						*cantMinas = casilleros * PORCENTAJE(P_FACIL);
						break;
				case 2: *cantUndo = U_MEDIO;
						*cantMinas = casilleros * PORCENTAJE(P_MEDIO);
						break;
				case 3: *cantUndo = U_DIFICIL;
						*cantMinas = casilleros * PORCENTAJE(P_DIFICIL);
						break;
				case 4: if(casilleros < 100) {
							printa("Este nivel requiere un minimo de 100 casilleros");
							// Dar la opcion de cambiar el tamaño?
							valido = FALSE;
						} else {
							*cantUndo = U_PESADILLA;
							*cantMinas = casilleros * PORCENTAJE(P_PESADILLA);
						}
						break;
				default:
						printa("Opcion invalida");
						valido = FALSE;
						break;
		}
	}
	while (!valido);
	return opcion;
}

int jugar(const TipoTablero incognita, int filas, int columnas, int cantMinas, int cantUndo) 
{
	TipoTablero tablero;

	tablero = generarTablero(filas, columnas);
	if (tablero == NULL)
	{
		return ERROR;
	}
	imprimirTablero(incognita, filas, columnas);

	TipoPos inicial;
	TipoPos final;

	inicial.fil = 5;
	inicial.col = 4;
	final.fil = 5;
	final.col = 5;

	//flag(tablero, inicial, final);

	printf("A\n");

	imprimirTablero(tablero, filas, columnas);

	int i;
	int * texto;
	texto = query(incognita, filas, columnas, 2, 1);

	for(i = 0; texto[i] != -1 ; i++) {
		printf("%d\t", texto[i]);
	}

	// Cuando termine el juego tengo que liberar el tablero
	//liberarMatriz(tablero, filas);

	return OK;
}

void imprimirEstado() {

}