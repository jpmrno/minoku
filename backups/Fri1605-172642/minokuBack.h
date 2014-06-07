#ifndef MINOKU_BACK_H
#define MINOKU_BACK_H

#define OK 1
#define ERROR 0
#define TRUE 1
#define FALSE 0

#define MINA '#'
#define FLAG '&'

typedef char * TipoLinea;
typedef TipoLinea * TipoTablero;

typedef struct {
	int i;
	int j;
} TipoPos;

// Funcion que genera una matriz de n filas por m columnas.
TipoTablero generarTablero(int filas, int columnas);

// Recibe el tablero y le agrega las "minas" (definidas por #define MINAS)
void llenarTablero(TipoTablero tablero, int filas, int columnas, int cantMinas);

// Revisa si existe una mina en (fil, col)
int tieneMina(const TipoTablero incognita, int fila, int col);

// Libera el espacio reservado para la matriz
void liberarMatriz(TipoTablero matriz, size_t filas);

int flag(TipoTablero tablero, TipoPos inicial, int cant, char filOcol);

#endif
