#ifndef _back_h
#define _back_h
/////////////////////////////////////////////////////
//////////////////////INCLUDES///////////////////////
/////////////////////////////////////////////////////
#include <stdio.h>
/////////////////////////////////////////////////////
/////////////////////DEFINICIONES////////////////////
/////////////////////////////////////////////////////
#define ERRORMALLOC NULL
enum {PERDIO_NIVEL=1, SIGA_JUGANDO, GANO_NIVEL};
enum {ERROR_SIN_MEMORIA=-14,ERROR_CORRUPCION,NOMBRE_INCORRECTO, NO_EXISTE, INVALIDO, UNDO_REPETIDO, ERROR_COLUMNA_VACIA, ERROR_FALTA_MARTILLO, ERROR_FALTA_HILERA, ERROR_FALTA_COLUMNA, ERROR_FUERA_DE_RANGO, JUGADA_INVALIDA, ERROR_CELDA_VACIA, ERROR_FILA_VACIA};
enum {MARTILLO=1,ELIMINAR,COLUMNA,HILERA,UNDO,QUIT,SAVE};
/////////////////////////////////////////////////////
/////////////////////ESTRUCTURAS/////////////////////
/////////////////////////////////////////////////////
typedef struct{
	int filas;
	int columnas;
	char ** tablero;
} tTablero;
typedef struct{
	int hileras;
	int columnas;
	int martillos;
} dispJugadas;

typedef struct{
	int maxNivel;
	int bitacora;
	int nivel;
	int puntuacion;

} guardarCargar;
/////////////////////////////////////////////////////
///////////////////DECLARACIONES/////////////////////
/////////////////////////////////////////////////////
int cargarPartida(char * nombre, guardarCargar * guaCar, tTablero * tablero, dispJugadas * bonuses,FILE ** bitacora);
void liberaMem(tTablero*tablero); /*libera memoria del tablero*/
int maxNivel(tTablero * tablero); /*calcula si el nivel maximo es 8 o filas*columnas/2*/
char ** creaMatriz (tTablero * tablero); /*hace espacio para la matriz*/
void creaTablero(int nivel, tTablero * tablero); /*hace el random y asigna a cada celda una letra*/
int bitacoraTablero(tTablero * tablero, FILE * bitacora); /*copia el tablero a la bitacora*/
int validaJugadaYEjecuta(char * orden,int coorFil,int coorCol, tTablero * tablero,int * azulejosBorrados, dispJugadas * bonuses, tTablero * undo, guardarCargar * guaCar, FILE ** bitacora);
/*verifica que la orden sea valida y si lo es ejecuta la jugada, si hay algun error retorna el error correspontiente*/
int guardarPartida(char * nombre, guardarCargar * guaCar, tTablero * tablero, dispJugadas * bonuses, FILE ** bitacora);
int deshacer(tTablero * tablero, tTablero * undo,dispJugadas * bonuses,int jugada); /* funcion de undo*/
int puntuacion(tTablero * tablero, int * azulejosBorrados); /*calcula la puntuacion de cada jugada*/
int bitacoraMovimientos(char * orden, FILE * bitacora, int puntuacion, int contador); /*guarda los movimientos en la bitacora*/
int gravedad (tTablero * tablero); /*hace que si hay celdas vacias caigan*/
int gravedadLateral (tTablero * tablero); /*hace que si hay columnas vacias caigan*/
int finDeNivel (tTablero * tablero, dispJugadas * bonuses); /*indica si no hay mas jugadas posibles*/
void cantBonuses(dispJugadas * bonuses, int puntNivel, tTablero * tablero); /*agrega si es necesario bonuses de acuerdo a los puntos del nivel*/
#endif



































