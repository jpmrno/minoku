#ifndef MINOKU_BACK_H
#define MINOKU_BACK_H

#define OK 1
#define ERROR 0
#define TRUE 1
#define FALSE 0

#define BLOQUE 5

#define MINA '#'
#define FLAG '&'
#define BARRIDO '-'

typedef char * TipoFila;
typedef TipoFila * TipoTablero;

typedef struct {
	int fil;
	int col;
} TipoPos;

// Funcion que genera una matriz de $filas por $columnas.
TipoTablero generarTablero(int filas, int columnas);

// Recibe el tablero y le agrega las "minas" (definidas por #define MINAS)
void llenarTablero(TipoTablero tablero, int filas, int columnas, int cantMinas);

// Pone BARRIDO (definido por #define BARRIDO) en la posicion
// Devuelve OK si no hay una mina
// Devuelve 0 si hubo una mina
int barrerCasillero(TipoTablero tablero, TipoPos elemento);

//
// Funciones del usuario
//
// Descripcion
void flag(TipoTablero tablero, TipoPos inicial, TipoPos final);
// Descripcion
int * queryMatrix(TipoTablero incognita, int c, int filas, int columnas);

// Libera el espacio reservado para la matriz
void liberarMatriz(TipoTablero matriz, size_t filas);

#endif
