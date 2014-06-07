#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "getnum.h"
#include "minokuBack.h"

// Resultado del juego //
enum resultado {PERDIO = 1, GANO, SIGUE};

//// Declaraciones ////

int juegoNuevo(void);
void modoJuego(tParametros * parametros, int * filas, int * columnas);
int jugarCampania(tTablero incognita, tTablero tablero, int filas, int columnas, tParametros *parametros, tEstado * eJuego, FILE * fp, int nivel);
int cargarJuego(void);
int jugar(const tTablero incognita, tTablero tablero, int filas, int columnas, tParametros * parametros, tEstado * eJuego);
int seguirJuego(tParametros * parametros, int casilleros, tEstado * eJuego);
void quit(const tTablero incognita, tTablero tablero, int filas, int columnas, tParametros * parametros);
void setDimTableros(int * filas, int * columnas);
int leerDificultad(int casilleros);
int leerFileCamp(tParametros * parametros, FILE ** fp);
void imprimirTablero(const tTablero tablero, int filas, int columnas);
void printa(const char s[]);
void printw(const char s[]);
void printError(int error);
void title(void);

int main(void) {
	int opcion, ret;
	char seguir;

	srand(time(NULL));

	title();

	do {
		printf("\n1) Juego Nuevo\n2) Cargar Juego\n3) Salir\n");
		opcion = getint(CYEL"Opcion: "CNRM);
		if(opcion >= 1 && opcion <= 3) {
			switch(opcion) {
				case 1:	
					ret = juegoNuevo();
					break;
				case 2: 
					ret = cargarJuego();
					break;
				case 3:	
					printw("Hasta luego!");
					return 0;
			}
			seguir = yesNo(CYEL"\n¿Desea jugar de nuevo? (s/n): "CNRM);
		}
		else {
			seguir = TRUE;
			printa("Opción incorrecta, vuelva a intentarlo");
		}
	} 
	while(seguir);

	return !ret;
}

int juegoNuevo(void) {
	tParametros parametros;
	tTablero incognita = NULL, tablero = NULL;
	tEstado eJuego = {0, 0, 0, FALSE, FALSE};
	int filas, columnas;
	FILE * fp = NULL;
	int ret, nivel = 1;

	// Se asume un juego nuevo no campania de movimientos infinitos
	parametros.movimientos = INF;
	parametros.campania = 0;
	modoJuego(&parametros, &filas, &columnas);
	// Juego dependiendo del modo
	if(parametros.campania) {
		if(!leerFileCamp(&parametros, &fp)) // Caso campania leo el nombre del archivo, si hay error (ver funcion) entonces ERROR
			return ERROR;
		ret = jugarCampania(incognita, tablero, filas, columnas, &parametros, &eJuego, fp, nivel);
		if(ret < OK) { // Caso que falle el prepararJuego
			printError(ret);
			return ERROR;
		}
	} else {
		ret = prepararJuego(&incognita, &tablero, filas, columnas, &parametros); // Inicializa los tableros y la dificultad
		if(ret < OK) {
			printError(ret);
			return ERROR;
		}
		ret = jugar(incognita, tablero, filas, columnas, &parametros, &eJuego);
		freeTablero(incognita, filas);
		freeTablero(tablero, filas);
		if(ret < OK) {
			printError(ret);
			return ERROR;
		}
	}
	return OK;
}

// Setea los parámetros basicos de cada modo
void modoJuego(tParametros * parametros, int * filas, int * columnas) {
	int opcion, valido;

	printw("¿Que modo desea jugar?");
	printf("1) Juego Individual sin limite de movimientos\n");
	printf("2) Juego Individual con limite de movimientos\n");
	printf("3) Juego por Campaña (con limite de movimientos)\n");
	do {
		valido = TRUE;
		opcion = getint(CYEL"Opcion: "CNRM);
		switch(opcion) {
			case 1: 
			case 2:	
				setDimTableros(filas, columnas);
				parametros->nivel = leerDificultad((*filas)*(*columnas));
				setDificultad(parametros, (*filas)*(*columnas));
				break;
			case 3:	
				parametros->campania = TRUE;
				return;
			default:
				valido = FALSE;
				printa("Opcion incorrecta, vuelva a intentarlo");
				break;
		}
		if(opcion == 2)
			parametros->movimientos = parametros->minas + parametros->undos;	
	}
	while(!valido);
}

int jugarCampania(tTablero incognita, tTablero tablero, int filas, int columnas, tParametros *parametros, tEstado * eJuego, FILE * fp, int nivel) {
	int ret = GANO;

	while(!feof(fp) && ret == GANO) {
		eJuego->flagsUsados = eJuego->flagsCorrectos = eJuego->sCorrectos = 0; 
		eJuego->barrioMina = FALSE;
		fscanf(fp, "%d\t%dx%d", &(parametros->nivel), &filas, &columnas);
		ret = prepararJuego(&incognita, &tablero, filas, columnas, parametros);
		if(ret < OK) {
			return ret;
		}
		parametros->movimientos = parametros->undos + parametros->minas;

		printf("Nivel: %d\n", nivel++);
		printf("Tablero de %dx%d: \n", filas, columnas);
		printf("Dificultad: %d\n", parametros->nivel);
		ret = jugar(incognita, tablero, filas, columnas, parametros, eJuego);
		freeTablero(incognita, filas);
		freeTablero(tablero, filas);
	}
	fclose(fp);

	if(ret == GANO)
		printw("Has ganado la campania");
	else if(ret == PERDIO)
		printw("Has perdido la campania");
	else 
		return ret;

	return OK;
}

int cargarJuego(void) {
	tParametros parametros;
	tTablero incognita = NULL, tablero = NULL;
	tEstado eJuego = {0, 0, 0, FALSE, FALSE};
	int filas, columnas;
	int auxDificultad, auxFilas, auxColumnas;
	tArchivo saveFile;
	FILE * fp;
	int ret, nivel = 1, error;

	do { // Lee el archivo guardado, en caso de error, imprime el mensaje correspondiente
		error = FALSE;
		printw("Introducir el nombre del archivo guardado (max. 20 caracteres)");
		scanf("%20[0-9a-zA-Z ]s",saveFile); //Lee hasta 20 caracteres y permite guardar nombres con espacios
		BORRA_BUFFER;
		ret = cargarDatos(&incognita, &tablero, &filas, &columnas, &parametros, saveFile);
		if(ret < OK) {
			printError(ret);
			if(ret != ERROR_OPEN_FILE || !yesNo("Quieres volver a intentarlo? (s/n): "))
				return ERROR;
			error = TRUE;
		}
	} while(error);

	analizarTablero(incognita, tablero, filas, columnas, &eJuego);

	if(parametros.campania) {
		fp = fopen(parametros.fileCamp, "rt"); // Si es campaña, entonces abre el archivo de esa campaña
		if(fp == NULL) {
			printError(ERROR_OPEN_FILE_CAMP); // En caso de que el archivo no exista tira error
			return ERROR;
		}

		do { // Avanza el archivo hasta el nivel en el que se encontraba cuando guardo
			fscanf(fp, "%d\t%dx%d", &auxDificultad, &auxFilas, &auxColumnas);
			nivel++;
		} while((parametros.nivel != auxDificultad || filas != auxFilas || columnas != auxColumnas) && !feof(fp));

		if(feof(fp)) { // En el caso que el archivo campania se haya modificado despues de guardar el juego y no encuentre el nivel
			printError(ERROR_CORRUPT_FILE_CAMP);
			return ERROR;
		}

		printf("\nNivel: %d\n", nivel - 1);
		printf("Tablero de %dx%d: \n", filas, columnas);
		printf("Dificultad: %d\n", parametros.nivel);
	}

	ret = jugar(incognita, tablero, filas, columnas, &parametros, &eJuego); // Juega el nivel guardado, ya sea campania o no
	freeTablero(incognita, filas);
	freeTablero(tablero, filas);

	if(parametros.campania && ret == GANO) // Sigue jugando el resto de la campaña en caso de ganar
		ret = jugarCampania(incognita, tablero, filas, columnas, &parametros, &eJuego, fp, nivel);
	if(ret < OK) {
		printError(ret);
		return ERROR;
	}
	return OK;
}

int jugar(const tTablero incognita, tTablero tablero, int filas, int columnas, tParametros * parametros, tEstado * eJuego) {
	char * comando;
	tComando lastCmd;
	tRunCmd toRun;
	int ret, error, i;

	if(parametros->movimientos == INF)
		eJuego->infMoves = TRUE;

	printw("Juego iniciado correctamente");

	while(seguirJuego(parametros, filas*columnas, eJuego) == SIGUE) {
		imprimirTablero(tablero, filas, columnas);
		printw("Te restan:");
		printf("%d flags.\n%d undos.\n", parametros->minas - eJuego->flagsUsados, parametros->undos);
		if(!eJuego->infMoves)
			printf("%d movimientos.\n", parametros->movimientos);
		putchar('\n');
		do {
			error = FALSE;
			ret = ERROR_CMD_NAME;
			printf(CYEL"Ingrese un comando: "CNRM);
			comando = leerString();
			if(comando != NULL)
				ret = validaCmd(incognita, tablero, filas, columnas, parametros, comando, &toRun);
			if(ret < OK) {
				printError(ret);
				error = TRUE;
			} else {
				ret = ejecutaCmd(incognita, tablero, filas, columnas, parametros, eJuego, &lastCmd, &toRun);
				if(ret < OK) {
					printError(ret);
					error = TRUE;
				}
			}
		} while(error);

		// Comandos que requieren printear algo:
		if(ret == QUIT) {
			quit(incognita, tablero, filas, columnas, parametros);
			return OK;
		}
		if(ret == QUERY) {
			printf(CYEL"\nSucesion de minas: "CNRM);
			if(toRun.queryVec[0] == EOA)
				printf("No hay minas");
			else 
				for( i = 0; toRun.queryVec[i] != EOA; i++)
					printf("%d  ", toRun.queryVec[i]);
			free(toRun.queryVec);
			putchar('\n');
		}
		if(eJuego->barrioMina && parametros->undos > 0) {
			printa("¡Barriste una mina!");
			printw("¡Pero todavia dispones de undos!");
			if(yesNo(CYEL"Deseas utilizar uno ahora? (Caso contrario pierdes...) "CNRM)) {
				toRun.cmd = UNDO;
				ret = ejecutaCmd(incognita, tablero, filas, columnas, parametros, eJuego, &lastCmd, &toRun);
			} else
				parametros->undos = 0;
		}


	}
	imprimirTablero(incognita, filas, columnas);
	return GANO;
}

int seguirJuego(tParametros * parametros, int casilleros, tEstado * eJuego) { //Verifica estado actual del tablero
	if(eJuego->barrioMina && parametros->undos == 0) {
		printw("HAS PERDIDO");
		return PERDIO;
	}
	if(!eJuego->infMoves) {
		if(parametros->movimientos < (casilleros - parametros->minas) - eJuego->sCorrectos)
			if(parametros->movimientos < parametros->minas - eJuego->flagsCorrectos) {
				printw("No tienes forma de ganar, HAS PERDIDO");
				return PERDIO;
			}
		if(parametros->movimientos == 0) {
			printw("Te quedaste sin movimientos, HAS PERDIO");
			return PERDIO;
		}
	}
	if(eJuego->sCorrectos == casilleros - parametros->minas) {
		printw("Barriste todas los casilleros sin minas, HAS GANADO");
		return GANO;
	}
	if(eJuego->flagsCorrectos == parametros->minas) {
		printw("Flagueaste todas las minas, HAS GANADO");
		return GANO;
	}

	return SIGUE;
}

void quit(const tTablero incognita, tTablero tablero, int filas, int columnas, tParametros * parametros) {
	tArchivo saveFile;
	int ret;

	if(yesNo(CYEL"\n¿Desea guardar la partida? (s/n): "CNRM))
		do {
			printw("Escriba el nombre del archivo a guardar");
			scanf("%20[0-9a-zA-Z ]s", saveFile);
			BORRA_BUFFER;
			ret = guardarDatos(incognita, tablero, filas, columnas, parametros, saveFile);
			if(ret < OK) {
				printError(ret);
				if(!yesNo(CYEL"Quieres volver a intentarlo? (s/n): "CNRM))
					return ;
			}
		} while(ret < OK);
}

void setDimTableros(int * filas, int * columnas) {
	int error;

	printw("Configuracion del tablero");
	do {
		*filas = getint(CYEL"Inserte la cantidad de filas (min. = %d, max. = %d): "CNRM, MIN_FIL, MAX_COL);
		if((error = (*filas < MIN_FIL || *filas > MAX_FIL)))
			printError(ERROR_DIM_TABLERO);
	} while(error);
	putchar('\n');
	do {
		*columnas = getint(CYEL"Inserte la cantidad de columnas (min. = %d, max. = %d): "CNRM, MIN_COL, MAX_COL);
		if((error = (*columnas < MIN_COL || *columnas > MAX_COL)))
			printError(ERROR_DIM_TABLERO);
	} while(error);
}

int leerDificultad(int casilleros) {
	int nivel, valido;

	do {
		valido = TRUE;
		printw(CYEL"Ingrese una dificultad"CNRM);
		printf("1) Facil\n2) Medio\n3) Dificil\n4) Pesadilla (requiere un minimo de 100 casilleros)\n");
		nivel = getint(CYEL"Opcion: "CNRM);
		if(nivel == 4 && casilleros < 100) {
			printa("Este nivel requiere un minimo de 100 casilleros");
			valido = FALSE;
		}
	} while(!valido || nivel <= 0 || nivel > 4);

	return nivel;
}

int leerFileCamp(tParametros * parametros, FILE ** fp) {
	do {
		printw("Introducir el nombre del archivo campania (max. 20 caracteres)");
		scanf("%20s", parametros->fileCamp); //No lee más de 20 caracteres
		BORRA_BUFFER;
		*fp = fopen(parametros->fileCamp, "rt");
		if(*fp == NULL) {
			printError(ERROR_OPEN_FILE);
			if(!yesNo("Quieres volver a intentarlo? (s/n): "))
				return ERROR;
		}
	} while(*fp == NULL);

	return OK;
}

void imprimirTablero(const tTablero tablero, int filas, int columnas) {
	int i, j, num;
	char letra = 'A';

	printf("\n\t");
	for ( num = 1; num <= columnas; num++)
		printf("%d\t", num);
	putchar('\n');
	
	for ( i = 0; i < filas; i++)
	{
		printf("%c\t", letra++);
		for ( j = 0; j < columnas; j++ ) {
			if(tablero[i][j] == FLAG)
				printf(CTGRN);
			if(tablero[i][j] == VACIO)
				printf(CBLU);
			if(tablero[i][j] == NO_TOCADO)
				printf(CTGREY);
			if(tablero[i][j] == MINA)
				printf(CTRED);
			printf("%c\t"CNRM, tablero[i][j]);
		}
		putchar('\n');
	}
	putchar('\n');
}

void printa(const char s[]) {
	printf(CTRED"\n~~~ %s ~~~\n\n"CNRM, s);
}

void printw(const char s[]) {
	printf(CTYEL"\n~~~ %s ~~~\n\n"CNRM, s);
}

void printError(int error) { //Imprime los diferentes mensajes de error
	switch(error) {
		case ERROR:	
			printa("Hubo un error");
			break;
		case ERROR_MALLOC:
			printa("Error en la (re)alocacion de memoria");
			break;
		case ERROR_CORRUPT_FILE:
			printa("El archivo esta corrupto");
			break;
		case ERROR_CORRUPT_FILE_CAMP:
			printa("El archivo campania esta corrupto");
			break;
		case ERROR_OPEN_FILE:
			printa("El archivo no existe");
			break;
		case ERROR_OPEN_FILE_CAMP:	
			printa("El archivo campania no existe");
			break;
		case ERROR_NAME_FILE:	
			printa("El nombre del archivo es invalido");
			break;
		case ERROR_CMD_NAME:	
			printa("Comando incorrecto o mal escrito");
			break;
		case ERROR_CMD_RANGE:	
			printa("EL rango del comando es incorrecto");
			break;
		case ERROR_MOVS:	
			printa("No tienes suficientes movimientos");
			break;
		case ERROR_FLAGS:	
			printa("No dispones de suficientes flags");
			break;
		case ERROR_UNDOS:
			printa("No dispones de mas undos");
			break;
		case ERROR_CMD_CANTDO:	
			printa("No puedes hacer esa operacion");
			break;
		case ERROR_S_VACIO:	
			printa("El casillero ya esta barrido");
			break;
		case ERROR_S_FLAG:	
			printa("El casillero tiene un flag");
			break;
		case ERROR_CASILLEROS:
			printa("Este nivel requiere mas casilleros");
			break;
		case ERROR_DIM_TABLERO:
			printa("El numero ingresado no esta permitido");
			break;
	}
}

void title(void) {
	printf(CTMAG" __   __  ___   __    _  _______  ___   _  __   __ \n");
	printf(CTBLU"|  |_|  ||   | |  |  | ||       ||   | | ||  | |  |\n");
	printf(CTMAG"|       ||   | |   |_| ||   _   ||   |_| ||  | |  |\n");
	printf(CTYEL"|       ||   | |       ||  | |  ||      _||  |_|  |\n");
	printf(CTGRN"|       ||   | |  _    ||  |_|  ||     |_ |       |\n");
	printf(CTRED"| ||_|| ||   | | | |   ||       ||    _  ||       |\n");
	printf(CTMAG"|_|   |_||___| |_|  |__||_______||___| |_||_______|\n"CNRM);
	printf("\t\tby... Juan Moreno & Gonzalo Ibars\n");
}