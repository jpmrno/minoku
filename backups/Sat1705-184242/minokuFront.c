#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "getnum.h"
#include "minokuBack.h"
#include "misc.h"

#define PORCENTAJE(x) ((double) (x)/100)

// Condiciones para el tablero
#define MIN_FIL 5
#define MIN_COL 5
#define MAX_FIL 19
#define MAX_COL 19
#define BLOQUE 5
#define INFMOVIMIENTOS -1
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

// Iniciales
int menuOpciones(void);

void setDimTablero(int * filas, int * columnas);

int setDificultad(TipoParametros *parametros, int casilleros);

void jugar(const TipoTablero incognita, TipoTablero tablero, int filas, int columnas, TipoParametros *parametros);

void imprimirEstado(TipoTablero tablero, int filas, int columnas, TipoParametros *parametros);

void imprimirTablero(TipoTablero tablero, int filas, int columnas);

int main(void) 
{
	TipoTablero incognita = NULL, tablero=NULL;
	int filas, columnas;
	TipoParametros parametros;
	int canProceed = TRUE;
	
	srand(time(NULL));
	

	setDimTablero(&filas, &columnas);
	setDificultad(&parametros, filas * columnas);
	incognita = generarTablero(filas, columnas);
	tablero = generarTablero(filas, columnas);
	if(incognita == NULL || tablero == NULL)
	{
		printa("No se pudo inicializar el juego");
		canProceed=FALSE;
	}
	if (canProceed) //Si no hubo errores hasta ahora
	{
		llenarTablero(incognita, filas, columnas, &parametros);

		jugar(incognita, tablero, filas, columnas, &parametros);
		liberarMatriz(incognita, filas);
	}
	return 0;
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

int setDificultad(TipoParametros *parametros, int casilleros)
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
				case 1: parametros -> cantUndo = U_FACIL;
						parametros -> cantMinas = casilleros * PORCENTAJE(P_FACIL);
						break;
				case 2: parametros -> cantUndo = U_MEDIO;
						parametros -> cantMinas = casilleros * PORCENTAJE(P_MEDIO);
						break;
				case 3: parametros -> cantUndo = U_DIFICIL;
						parametros -> cantMinas = casilleros * PORCENTAJE(P_DIFICIL);
						break;
				case 4: if(casilleros < 100) {
							printa("Este nivel requiere un minimo de 100 casilleros");
							valido = FALSE;
						} else {
							parametros -> cantUndo = U_PESADILLA;
							parametros -> cantMinas = casilleros * PORCENTAJE(P_PESADILLA);
						}
						break;
				default:
						printa("Opcion invalida");
						valido = FALSE;
						break;
		}
		parametros -> movimientos=parametros -> cantMinas + parametros -> cantUndo;
		parametros -> flags=parametros -> cantMinas;
	}
	while (!valido);
	return opcion;
}

void imprimirEstado(TipoTablero tablero, int filas, int columnas, TipoParametros *parametros)
{
	if(parametros -> movimientos != INFMOVIMIENTOS)	
		printf("Cantidad de movimientos restantes: %d\n", parametros -> movimientos);
	printf("Cantidad de flags restantes: %d\n", parametros -> flags);
	printf("Cantidad de Undo restantes: %d\n", parametros -> cantUndo);
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

void jugar(const TipoTablero incognita, TipoTablero tablero, int filas, int columnas, TipoParametros *parametros) //NO TERMINADO
{
	char * comando;k
	TipoTablero auxTablero;
	//while(parametros -> movimientos != 0) {
		imprimirEstado(tablero, filas, columnas, parametros);
		auxTablero = copiaTablero();
		comando=leerComando();
		realizarComando();
	//}
	imprimirTablero(incognita, filas, columnas); //Claramente se saca al final del proyecto
}

TipoTablero copiaTablero(TipoTablero tablero, int filas, int columnas) 
{
	TipoTablero auxTablero;
	int i, j;
	
	auxTablero = generarTablero(filas, columnas);

	if (auxTablero != NULL)
	{
		for(i = 0; i < filas; i++) 
		{
			for(j = 0; j < columnas; j++) 
			{
				auxTablero[i][j] = tablero[i][j];
			}
		}
	}
	return auxTablero;
}

char *leerComando()
{
	char c, valid;
	char *comando=NULL;

	do
	{
		valid=TRUE;
		while ((c=getchar()) != '\n')
		{
			if(dim%BLOQUE == 0)
				if(reallocVector(vector, sizeof(*comando)) == NULL)
					valid=FALSE;
			if(valid)
				comando[dim++]=c;
		}
		if (valid) //DEJA DE LEER Y PROCEDE A VALIDAR EL COMANDO	
			if (valid == validaComando(comando))
				return comando;
	}
	while(!valid);
}
