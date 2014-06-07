#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "getnum.h"
#include "minokuBack.h"

#define PORCENTAJE(x) ((double) (x)/100)

// Condiciones para el tablero
#define MIN_FIL 5
#define MIN_COL 5
#define MAX_FIL 19
#define MAX_COL 19
#define BLOQUE 5
//Opciones de juego
#define LOADGAME 1
#define EXIT 2
#define SINLIMITE 3
#define CONLIMITE 4
#define CAMPANIA 5
// Condiciones para la dificultad
#define P_FACIL 20
#define P_MEDIO 50
#define P_DIFICIL 70
#define P_PESADILLA 90
#define U_FACIL 10
#define U_MEDIO 5
#define U_DIFICIL 3
#define U_PESADILLA 1
//Valores de verdad
#define OK 1
#define ERROR 0
#define TRUE 1
#define FALSE 0

// Iniciales
void setDimTablero(int * filas, int * columnas);

int setDificultad(int * cantUndo, int * cantMinas, int casilleros);

int jugar(const TipoTablero incognita, int filas, int columnas, int cantMinas, int cantUndo, int movimientos, int opcion, int flags);

void imprimirTablero(TipoTablero tablero, int filas, int columnas, int opcion, int movimientos, int flags, int cantUndo);

void main() 
{
	TipoTablero incognita=NULL;
	int filas, columnas;
	int cantMinas, movimientos, cantUndo;
	int opcion, resultado, canProceed, flags;
	
	srand(time(NULL));
	
	do
	{
		canProceed=TRUE;
		opcion=menuOpciones();
		
		if (opcion==SINLIMITE || opcion==CONLIMITE)
		{
			setDimTablero(&filas, &columnas);
			incognita = generarTablero(filas, columnas);
			if(incognita == NULL)
			{
				printa("No se pudo inicializar el juego");
				canProceed=FALSE;
			}
			if (canProceed) //Si no hubo errores hasta ahora
			{
				setDificultad(&cantUndo, &cantMinas, filas * columnas);
				llenarTablero(incognita, filas, columnas, cantMinas);

				resultado = jugar(incognita, filas, columnas, cantMinas, cantUndo, movimientos, opcion, flags);
				if ( resultado==OK )
					printa("Juego terminado con exito");
				else
					printa("Hubo un error en el juego");
				liberarMatriz(incognita, filas);
			}
		}
		
		/*else if(opcion==LOADGAME)
		{
			//FALTA AGREGAR CÓDIGO
		}
		*/
		
	}
	while(opcion!=EXIT);
	printa("Gracias por jugar");
	return;
}

int menuOpciones()
{
	printf("-------------------\n");
	printf("Bienvenido a Minoku\n");
	printf("-------------------\n");

	char c, valido;

	do
	{
		printf("1 - Juego Nuevo\n2 - Cargar Juego\n3 - Terminar Juego\n");
		c=getint("Ingrese una de las siguiente opciones: ");
		switch(c)
		{
			case 1:		valido=TRUE;
						break;
			case 2:		return LOADGAME;
			case 3:		return EXIT;
			default:	valido=FALSE;
						break;
		}
	}
	while(!valido);
	do
	{
		valido=TRUE;
		printf("1 - Juego Nuevo\n");
		printf("\t1.1 - Juego individual sin límite de movimientos\n");
		printf("\t1.2 - Juego individual con límite de movimientos\n");
		printf("\t1.3 - Juego por campaña ( Siempre con límite de movimientos\n");
		c=getint("Inserte una de las siguiente opciones: ");
		switch(c)
		{
			case '1':	return SINLIMITE;
			case '2':	return CONLIMITE;
			case '3':	return CAMPANIA;
			default:	valido=FALSE;
		}
	}
	while (!valido);
}

void setDimTablero(int * filas, int * columnas)
{
	do
		*filas = getint("Inserte la cantidad de filas (Min. = %d)(Max. = %d): ", MIN_FIL, MAX_COL);
	while (*filas < MIN_FIL || *filas > MAX_FIL);
	do
		*columnas = getint("Inserte la cantidad de filas (Min. = %d)(Max. = %d): ", MIN_COL, MAX_COL);
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

void imprimirTablero(TipoTablero tablero, int filas, int columnas, int opcion, int movimientos, int flags, int cantUndo) //AGREGO PARA QUE IMPRIMA OTROS PARAMETROS
{
	int i, j, num;
	char letra = 'A';

	if(opcion == SINLIMITE)	
		printf("Cantidad de movimientos restantes: %d\n", movimientos);
	printf("Cantidad de flags restantes: %d\nCantidad de Undo restantes: %d\n", flags, cantUndo);
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

int jugar(const TipoTablero incognita, int filas, int columnas, int cantMinas, int cantUndo, int movimientos, int opcion, int flags) //NO TERMINADO
{
	TipoTablero tablero;
	TipoPos inicial, final;
	char c, puedeSeguir, programError, undoFlag;
	char *comando=NULL;
	int dim;

	tablero = generarTablero(filas, columnas);
	if (tablero == NULL)
		return ERROR;
	
	// imprimirTablero(incognita, filas, columnas); //Claramente se saca al final del proyecto
	imprimirTablero(tablero, filas, columnas, opcion, movimientos, flags, cantUndo);
	while (movimientos>0 && puedeSeguir)
	{
		dim=0;
		while ((c=getchar()) !='\n' && !programError)
		{
			if (dim%BLOQUE == 0)
			{
				comando=realloc(comando, (dim+BLOQUE)*sizeof(*comando));
				if (comando == NULL)
					programError=TRUE;
			}
			if (!programError) // Si hubo, que vuelva a pedir comando
				comando[dim++]=c;
		}
		if (!programError)
		{
			comando[dim]='\0';
			if(validaComando(comando))
			{
				puedeSeguir=realizarComando(comando, &undoFlag);
				if(opcion == CONLIMITE)
					movimientos--;
				imprimirTablero(tablero, filas, columnas, opcion, movimientos, flags, cantUndo);
			}
		}	
		free(comando);
	}
}
