#ifndef MINOKU_BACK_H
#define MINOKU_BACK_H

//// DEFINICIONES ////

enum comandos {FLAG = 1, UNFLAG, QUERY, S, SAVE, UNDO, QUIT};

#define TRUE 1
#define FALSE 0

// Errores //
#define OK 1
#define ERROR 0
#define ERROR_MALLOC -1
#define ERROR_CORRUPT_FILE -2
#define ERROR_OPEN_FILE -3
#define ERROR_NAME_FILE -4
#define ERROR_CMD_NAME -5
#define ERROR_CMD_RANGE -6
#define ERROR_MOVS -7
#define ERROR_FLAGS -8
#define ERROR_CMD_CANTDO -9
#define ERROR_S_VACIO -10
#define ERROR_S_FLAG -11
#define ERROR_CORRUPT_FILE_CAMP -12
#define ERROR_UNDOS -13
#define ERROR_OPEN_FILE_CAMP -14
#define ERROR_CASILLEROS -15
#define ERROR_DIM_TABLERO -16

// Condiciones para el tablero //
#define MIN_FIL 5
#define MIN_COL 5
#define MAX_FIL 19
#define MAX_COL 19

// Fichas de tablero //
#define MINA '#'
#define FLAG '&'
#define VACIO '-'
#define NO_TOCADO '0'

// Condiciones para la dificultad //
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

// Instrucciones //
#define PORCENTAJE(x) ((double)(x)/100)
#define SWAP(a,b,c) {c=a; a=b; b=c;}
#define BORRA_BUFFER while (getchar() != '\n')

// Miscelaneo //
#define BLOQUE 5
#define EOA -1
#define INF 0
#define MAX_FILE_NAME 21

// Colores //
#define CNRM  "\x1B[0m"
#define CTRED  "\x1B[1;31m"
#define CTGRN  "\x1B[1;32m"
#define CTYEL  "\x1B[1;33m"
#define CYEL  "\x1B[33m"
#define CBLU  "\x1B[34m"
#define CTBLU  "\x1B[1;34m"
#define CTMAG  "\x1B[1;35m"
#define CTGREY  "\x1B[1;30m"
#define CTCYN  "\x1B[1;36m"
#define CWHT  "\x1B[37m"


//// ESTRUCTURAS Y SINONIMOS A TIPOS DE DATO ////

typedef char *tFila;
typedef tFila *tTablero;

typedef char tArchivo[MAX_FILE_NAME];

typedef struct {
	char fil;
	int col;
} tElemento;

typedef struct {
	int nivel;
	int movimientos;
	int undos;
	int minas;
	int campania;
	tArchivo fileCamp;
} tParametros;

typedef struct {
	int flagsUsados;
	int flagsCorrectos;
	int sCorrectos;
	int barrioMina;
	int infMoves;
} tEstado;

typedef struct {
	int cmd;
	char fil;
	int col;
} tComando;

typedef struct {
	int cmd;
	tElemento ini;
	tElemento fin;
	char * saveFile;
	int * queryVec;
} tRunCmd;



//// DECLARACIONES ////

// Manejo de matrices //

tTablero generarTablero(int filas, int columnas);
/*Genera una matriz dinámica de n filas y m columnas*/
void freeTablero(tTablero tablero, int filas);
/*Libera la memoria reservada para una matriz generada previamente*/
char * leerString(void);
/*Guarda en un string los caracteres insertados por entrada estándar (dinamicamente)*/
int setDificultad(tParametros *parametros, int casilleros);
/*Setea la cantidad de undos, minas a partir del nivel de dificultad que recibe*/
int guardarDatos(tTablero incognita, tTablero tablero, int filas, int columnas, tParametros * p, const tArchivo fileSave);
/*Guarda en un archivo fileSave los datos de la partida actual*/
int cargarDatos(tTablero * incognita, tTablero * tablero, int * filas, int * columnas, tParametros * p, tArchivo saveFile);
/*Carga los datos de una partida de cualquier tipo previamente guardada y verifica que los datos que contenga no estén corruptos*/
int prepararJuego(tTablero *incognita, tTablero *tablero, int filas, int columnas, tParametros *parametros);
/*Genera el tablero para jugar y el incognita y a éste se le distribuyen minas, setea la dificultad*/
void llenarTablero(tTablero tablero, int filas, int columnas, char obj);
/*Coloca en todas las posiciones el objeto indicado por parámetro*/
void llenarMinas(tTablero tablero, int filas, int columnas, tParametros *parametros);
/*En un juego nuevo, distribuye de manera pseudoaleatoria las minas en un tablero*/
void analizarTablero(tTablero incognita, tTablero tablero, int filas, int columnas, tEstado * eJuego);
/*Se comparan los 2 tableros para saber cuantos flags colocados coinciden con las minas del incognita*/ 
int validaCmd(const tTablero incognita, tTablero tablero, int filas, int columnas, tParametros * p, char * comando, tRunCmd * toRun);
/*Valida el comando previamente ingresado por el usuario. Devuelve un entero que hace referencia al comando o un error correspondiente en caso contrario*/
int ejecutaCmd(const tTablero incognita, tTablero tablero, int filas, int columnas, tParametros * p, tEstado * eJuego, tComando * lastCmd, tRunCmd * toRun);
int rangoValido(tElemento * inicial, tElemento * final, int filas, int columnas);
/*Valida el rango de lo ingresado en comandos específicos. En caso de que se hayan pasado los valores al revés, se intercambian*/
int posValida(tElemento * elemento, int filas, int columnas);
/*Comando auxiliar de rangoValido que verifica cada cota por separado*/

// Comandos del juego //

int flag(const tTablero incognita, tTablero tablero, tElemento *elemento, tEstado * juego);
int unflag(const tTablero incognita, tTablero tablero, tElemento * elemento, tEstado * eJuego);
int barrerCasillero(tTablero incognita, tTablero tablero, tElemento * elemento, tEstado * eJuego);
int query(tTablero incognita, int filas, int columnas, tElemento * ini, tRunCmd * toRun);

// Generador de número pseudoaleatorio //

double randNormalize(void);
/*Distribuye de manera uniforme la generación de números pseudoaleatorio elegidos*/
int randInt(int izq, int der);

#endif