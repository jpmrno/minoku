#ifndef MINOKU_BACK_H
#define MINOKU_BACK_H

#define OK 1
#define ERROR 0
#define ERROR_MALLOC -1
#define ERROR_CORRUPT_FILE -2
#define ERROR_OPEN_FILE -3
#define ERROR_NAME_FILE -4

#define TRUE 1
#define FALSE 0

#define BLOQUE 5
#define EOA -1

#define MINA '#'
#define FLAG '&'
#define VACIO '-'
#define NO_TOCADO '0'

// Condiciones para la dificultad
// % de Minas
#define P_FACIL 20
#define P_MEDIO 50
#define P_DIFICIL 70
#define P_PESADILLA 90
// Cantidad de Undos
#define U_FACIL 10
#define U_MEDIO 5
#define U_DIFICIL 3
#define U_PESADILLA 1

// Condiciones para el tablero
#define MIN_FIL 5
#define MIN_COL 5
#define MAX_FIL 19
#define MAX_COL 19
#define INF 0

#define BLOQUE 5

#define PORCENTAJE(x) ((double) (x)/100)

typedef char * TipoFila;
typedef TipoFila * TipoTablero;

typedef struct 
{
	int fil;
	int col;
} TipoPos;

typedef struct 
{
	TipoPos inicial;
	TipoPos final;
} TipoIntervalo;

typedef struct 
{
	int dificultad;
	int movimientos;
	int flags;
	int cantUndo;
	int cantMinas;
	int campania;
} TipoParametros;

typedef struct 
{
	TipoTablero tablero;
	int flagsTotales;
	int flagsCorrectos;
	int sCorrectos;
} TipoUndo;

void guardarDatos(FILE *fp, TipoTablero incognita, TipoTablero tablero, int filas, int columnas, TipoParametros * p, char * fileCampania);
int cargarDatos(TipoTablero * incognita, TipoTablero * tablero, int * filas, int * columnas, TipoParametros * p, char * fileCamp, char * fileSaved);
//
int prepararJuego(TipoTablero * incognita, TipoTablero * tablero, int filas, int columnas, TipoParametros * parametros);
// Genera una matriz de $filas * $columnas
TipoTablero generarTablero(int filas, int columnas);

// Llena la matriz con MINAs y completa el resto con VACIO
void llenarMinas(TipoTablero tablero, int filas, int columnas, TipoParametros *parametros);
// Llena la matriz de NO_TOCADOs
void llenarTablero(TipoTablero tablero, int filas, int columnas, char obj);
// 
int setDificultad(TipoParametros *parametros, int casilleros);

// Crea una matriz identica y devuelve su pos. de memoria
TipoTablero copiaTablero(TipoTablero tablero, int filas, int columnas);

// 
void flag(TipoTablero tablero, TipoPos inicial, TipoPos final, TipoParametros *parametros);
//
void unflag(TipoTablero tablero, TipoPos inicial, TipoPos final, TipoParametros *parametros);
//
int barrerCasillero(TipoTablero incognita, TipoTablero tablero, TipoPos elemento);
//
int * query(TipoTablero incognita, int filas, int columnas, int num, char doFil);

// 
void liberarMatriz(TipoTablero matriz, size_t filas);
//
void * reallocVector(void *vector, int dim, size_t size);

char * leerString(void);

#endif