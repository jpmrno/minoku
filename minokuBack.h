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

// Miscelaneo //
#define BLOQUE 5
#define EOA -1
#define INF 0
#define MAX_FILE_NAME 21

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
	tElemento pos;
} tComando;

typedef struct {
	int cmd;
	tElemento ini;
	tElemento fin;
	char * saveFile;
	int * queryVec;
} tRunCmd;

//// DECLARACIONES ////

// Libera la memoria reservada para una matriz generada previamente
void freeTablero(tTablero tablero, int filas);
// Guarda en un string los caracteres insertados por entrada estándar (dinamicamente)
char * leerString(void);
// Setea la cantidad de undos, minas a partir del nivel de dificultad que recibe
int setDificultad(tParametros *parametros, int casilleros);
// Guarda en un archivo fileSave los datos de la partida actual
int guardarDatos(const tTablero incognita, const tTablero tablero, int filas, int columnas, tParametros * p, const tArchivo fileSave);
// Carga los datos de una partida de cualquier tipo previamente guardada y verifica que los datos que contenga no estén corruptos
int cargarDatos(tTablero * incognita, tTablero * tablero, int * filas, int * columnas, tParametros * p, tArchivo saveFile);
// Genera el tablero para jugar y el incognita y a éste se le distribuyen minas, setea la dificultad
int prepararJuego(tTablero *incognita, tTablero *tablero, int filas, int columnas, tParametros *parametros);
// Se comparan los 2 tableros para saber cuantos flags colocados coinciden con las minas del incognita
void analizarTablero(const tTablero incognita, const tTablero tablero, int filas, int columnas, tEstado * eJuego);
// Valida el comando previamente ingresado por el usuario. Devuelve un entero que hace referencia al comando o un error correspondiente en caso contrario
int validaCmd(const tTablero incognita, tTablero tablero, int filas, int columnas, tParametros * p, char * comando, tRunCmd * toRun);

int ejecutaCmd(const tTablero incognita, tTablero tablero, int filas, int columnas, tParametros * p, tEstado * eJuego, tComando * lastCmd, tRunCmd * toRun);

#endif