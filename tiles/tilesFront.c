/////////////////////////////////////////////////////
//////////////////////INCLUDES///////////////////////
/////////////////////////////////////////////////////
#include <stdio.h>
#include "getnum.h"
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "tilesBack.h"
/////////////////////////////////////////////////////
/////////////////////DEFINICIONES////////////////////
/////////////////////////////////////////////////////
#define BLOQUE 3
#define BITACORA 1
#define SI 1
#define NO 0
//////////////////////////////////////////
////////////DECLARACIONES/////////////////
//////////////////////////////////////////
void juegoNuevo(int flagBitacora, int flagCargar);
void instrucciones(void);
char * stringPrimero(void);
void imprimeErrores(int error);
void dimension(tTablero * tablero);
void cantNiveles(int * cantNiv, int maxNivel);
void imprimeTablero (tTablero * tablero);
int salir(void);
int menu();
//////////////////////////////////////////
////////////////MAIN//////////////////////
//////////////////////////////////////////
int
main ()
{
	while (menu()==0); /*Cuando la función menú devuelve cero significa que el usuario salió de juego.*/
	return 0;
}
//////////////////////////////////////////
/////////////FUNCIONES////////////////////
//////////////////////////////////////////
int menu()
{
	int num;
	do
		num=getint("\n ¡¡¡¡Bienvenido a Azulejos!!!!\n\n1. Juego Nuevo.\n2. Juego Nuevo con Bitácora.\n3. Cargar juego guardado.\n4. Instrucciones.\n5. Salir del juego.\n\nElija un número entre 1 y 5.\n");
	while (num>5 || num<1);
		switch (num)
			{
			case 1: juegoNuevo(0,0); /*Inicia un nuevo juego sin bitácora.*/
				break;

			case 2: juegoNuevo(1,0); /*Inicia un nuevo juego con bitácora.*/
				break;

			case 3:	juegoNuevo(0,1); /*Inicia un juego guardado.*/
				break;

			case 4: instrucciones(); /*Muestra las instrucciones.*/
				break;

			case 5: return 1; /*Cierra el juego*/
			}
	return 0;
}

void juegoNuevo(int flagBitacora, int flagCargar)
{
	char * nombre, * nomPartida, **pperror;
	tTablero tablero, undo;
	guardarCargar guaCar;
	int coorCol=0,coorFil=0,lvl,azulejosBorrados=0,puntuacionJugada=0,puntuacionNivel=0,fin=GANO_NIVEL, numJugada,jugadaAux=-1,jugada=-1,niv=1,nivMax,error,puntuacionAuxiliar=0;
	char * orden=NULL;
	FILE * bitacora;
	dispJugadas bonuses={1,1,1}; /*Inicializa las jugadas especiales en 1*/
	if (flagCargar==1) /*Si es verdadero prepra una partida guardada, de lo contrario prepara una nueva partida*/
	{
		printf("Ingrese el nombre de la partida que desea cargar.\n");
		nombre=stringPrimero();
		error=cargarPartida(nombre, &guaCar, &tablero, &bonuses, &bitacora);
		imprimeErrores(error);
		if(error==NO_EXISTE || error==ERROR_SIN_MEMORIA) /*Si es verdadero vuelve al menú*/
			return;
		else if(error==ERROR_CORRUPCION) /*Si es verdero vuelve al menú pero antes libera la memoria que se guardó*/
		{
			liberaMem(&tablero);
			return;
		}
		if (guaCar.bitacora==1) /*Ve si el juego cargado es con bitácora*/
			flagBitacora=BITACORA; 
		else /*Si es sin bitácora libera memoria y asigna los demás datos del juego cargado a las variables correspondientes*/
			free(nombre);
		lvl=guaCar.maxNivel;
		niv=guaCar.nivel;
		puntuacionNivel=guaCar.puntuacion;
	}
	else
	{
		dimension (&tablero);
		nivMax=maxNivel(&tablero); 
		cantNiveles(&lvl,nivMax); 
		guaCar.maxNivel=lvl; 
		pperror=creaMatriz(&tablero);
		if(pperror==ERRORMALLOC)
		{
			printf("No hay memoria suficiente, intente con dimensiones menores.\n");
			return;
		}
	}
	if (flagBitacora==BITACORA && flagCargar==0)
		bitacora=fopen("bitacora.txt","wt");
	guaCar.bitacora=flagBitacora;
	undo.filas=tablero.filas;
	undo.columnas=tablero.columnas;
	pperror=creaMatriz(&undo);
	if(pperror==ERRORMALLOC)
	{
		printf("No hay memoria suficiente.\n");
		return;
	}
	while(niv<=lvl && fin==GANO_NIVEL) /*Sale del ciclo cuando pasamos a un nivel mayor al máximo*/
	{
		fin=SIGA_JUGANDO;
		if (flagCargar==0)
			creaTablero(niv,&tablero);
		printf("Nivel %d\n",niv);
		imprimeTablero (&tablero);
		printf("Martillazos: %d\tHileras: %d\tColumnas: %d\n",bonuses.martillos,bonuses.hileras,bonuses.columnas);
		if (flagBitacora==BITACORA)
		{
			bitacoraTablero(&tablero,bitacora);
			numJugada=1;
		}
		while(fin==SIGA_JUGANDO) /*Entra al ciclo mientras se puedan seguir haciendo jugadas en el nivel en el que estamos*/
		{
			orden=stringPrimero();
			if (orden!=NULL) /*Si es verdadero valida la jugada y la ejecuta, de lo contrario pide tomar de nuevo la orden*/
			{
				guaCar.puntuacion=puntuacionNivel;
				guaCar.nivel=niv;
				jugada=validaJugadaYEjecuta(orden,coorFil,coorCol,&tablero,&azulejosBorrados,&bonuses, &undo,&guaCar, &bitacora);
				imprimeErrores(jugada);
				if (jugada==QUIT) /*Si es verdadero pregunta si se desea guardar el juego antes de salir*/
				{
					if(salir()==SI)
					{
						do
						{
							printf("Escriba el nombre con el que quiere guardar la partida.\n");
							nomPartida=stringPrimero();
						}
						while (guardarPartida(nomPartida, &guaCar, &tablero, &bonuses, &bitacora)==NOMBRE_INCORRECTO);
						if(guaCar.bitacora!=1)
							free(nomPartida);
					}
					if(flagBitacora==BITACORA)
						fclose(bitacora);
					free(orden);
					liberaMem(&tablero);
					liberaMem(&undo);
					return;
				}
				if (jugada==UNDO && puntuacionNivel!=0 && flagCargar==0) /*Hace el undo en el caso de que no se haya hecho apenas comenzamos un nivel*/
				{
					deshacer(&tablero,&undo,&bonuses,jugadaAux);
					puntuacionNivel-=puntuacionAuxiliar;
					puntuacionJugada=0;
				}
				else if (jugada>=MARTILLO && jugada<QUIT) 
				{
					jugadaAux=jugada;
					puntuacionJugada=puntuacion(&tablero,&azulejosBorrados);
					puntuacionAuxiliar=puntuacionJugada;
					puntuacionNivel+=puntuacionJugada;
				}
				else
					puntuacionJugada=0;
				if(flagCargar==1)
				{
					flagCargar=0;
					numJugada=1;
				}
				if (flagBitacora==BITACORA)
				{
					bitacoraMovimientos(orden,bitacora,puntuacionJugada,numJugada);
					numJugada++;
				}
				free(orden);
			}
			else
				printf("No se tomó la orden.\n");
			printf("Cantidad de azulejos:%d\nPuntuacion acumulada:%d\nPuntuacion de la jugada:%d\n",azulejosBorrados,puntuacionNivel,puntuacionJugada);
			gravedad (&tablero);
			gravedadLateral (&tablero);
			azulejosBorrados=0;
			if ((fin=finDeNivel (&tablero,&bonuses))!=GANO_NIVEL)
			{
				imprimeTablero (&tablero);
				printf("Martillazos: %d\tHileras: %d\tColumnas: %d\n",bonuses.martillos,bonuses.hileras,bonuses.columnas);
			}
		}
		cantBonuses(&bonuses,puntuacionNivel,&tablero);
		if(fin==GANO_NIVEL)
			{
			niv++;
			}
		puntuacionNivel=0;
		if(niv<=lvl && fin==GANO_NIVEL)
			printf("Ha ganado el nivel %d\n\nNuevo nivel:%d\n",niv-1,niv);
	}
	if(niv>lvl)
		printf("\n¡¡¡¡¡Felicitaciones, ha ganado el juego!!!!!\n\n");
	else
		printf("\nQue lástima, has perdido. Prueba con menos niveles o una pared más pequeña en tu próximo juego.\n");
	if (flagBitacora==BITACORA)
		fclose(bitacora);
	liberaMem(&tablero);
	liberaMem(&undo);
	return;
}

void instrucciones(void)
{
	printf("\t\t\t\t\tINSTRUCCIONES:\n\n");
	printf("\t\t-Al comenzar el juego a usted se le presentará un tablero\n\t\tde juego relleno con letras y coordenadas ('y' fila, 'x' columna)\n\t\tpara localizar la ubicación de cada una de estas.\n");
	printf("\t\t-Cada letra del abecedario represanta un color distinto de\n\t\tazulejo y el tablero vendría a ser la pared en donde se encuentran.\n");
	printf("\t\t-El objetivo del juego es lograr ganar el nivel máximo.\n");
	printf("\t\t-Primero se le pedirán las dimensiones de la pared y la cantidad\n\t\t\tde niveles que desea jugar.\n");
	printf("\t\t-Para pasar de nivel usted debe eliminar todos los azulejos\n\t\tpresentes mediante el uso de los siguientes movimientos:\n");
	printf("\t\t1) Eliminar (e y,x): elimina todo azulejo adyacente del\n\t\t\t   mismo color al que usted seleccionó. Es ilimitado.\n");
	printf("\t\t2) Eliminar Columna (c x): elimina toda la columna seleccionada. Es limitado.\n\t\t3) Eliminar Hilera (h y): elimina toda la hilera seleccionada. Es limitado\n");
	printf("\t\t4) Martillazo (m y,x): elimina el azulejo seleccionado junto\n\t\t\t   con los 8 que lo rodean. Es limitado.\n");
	printf("\t\t-Usted puede perder si no puede aplicar Eliminar y ya no tiene\n\t\t\tmovimientos limitados para usar.\n\n");
	printf("\t\t-También puede usar los comandos Undo (undo), Save (save ...) y Quit (quit).\n");
	printf("\t\tNota 1: Existe gravedad (los azulejos caerán a los lugares vacios).\n");
	printf("\t\tNota 2: Existe gravedad lateral (si existe una columna vacia las columnas a\n\t\t\tla derecha que tengan azulejos se moveran hacia la izquierda).\n");
	printf("\t\tNota 3: Si usted consigue buen puntaje en sus movimientos al finalizar\n\t\t\tel nivel se le otorgará un bonus de los movimientos limitados.\n");
}

char * stringPrimero(void)
{
	char *orden=NULL,*aux=NULL;
	int i=1,c;

	if ((c=getchar())=='\n')
		return NULL;
	orden=malloc(BLOQUE);
	if(orden==NULL)
		return NULL;
	*(orden)=c;
	while((c=getchar())!='\n')
	{
		if (i%BLOQUE==0)
		{
			aux=realloc(orden,BLOQUE+i);
			if (aux==NULL)
			{
				free(orden);
				return NULL;
			}
			orden=aux;
		}
		*(orden+i)=c;
		i++;
	}
	if (i%BLOQUE==0)
	{
		aux=realloc(orden,i+1);
		if (aux==NULL)
		{
			free(orden);
			return NULL;
		}
		orden=aux;
	}
	*(orden+i)=0;
	return orden;
}

void imprimeErrores(int error)
{
	switch(error)
	{
		case ERROR_FUERA_DE_RANGO:	printf("ERROR: Coordenadas fuera de rango.\n");
                                        	break;
       		case INVALIDO:              	printf("ERROR: Comando inválido.\n");
                                        	break;
      		case ERROR_CELDA_VACIA:     	printf("ERROR: La celda se encuentra vacía.\n");
      	                                  	break;
      		case ERROR_FILA_VACIA:      	printf("ERROR: La fila se encuentra vacía.\n");
                                        	break;
            	case ERROR_FALTA_COLUMNA:   	printf("ERROR: No dispone de más jugadas Columna.\n");
                                        	break;
            	case ERROR_FALTA_HILERA:    	printf("ERROR: No dispone de más jugadas Hilera.\n");
                                        	break;
            	case ERROR_FALTA_MARTILLO:  	printf("ERROR: No dispone de más jugadas Martillazo.\n");
                                        	break;
            	case ERROR_COLUMNA_VACIA:   	printf("ERROR: La columna se encuentra vacía.\n");
                                        	break;
            	case UNDO_REPETIDO:         	printf("ERROR: No puede hacer Undo dos veces seguidas.\n");
                                        	break;
            	case NO_EXISTE:             	printf("ERROR: El archivo que desea cargar no existe.\n");
                                        	break;
            	case NOMBRE_INCORRECTO:     	printf("ERROR: Escriba un nombre válido (intente con letras y/o números).\n");
                                        	break;
                case JUGADA_INVALIDA:		printf("ERROR: La jugada eliminar no se puede llevar a cabo en esas coordenadas.\n");
                				break;
                case ERROR_CORRUPCION:		printf("ERROR: El archivo está corrupto.\n");
                				break;
		case ERROR_SIN_MEMORIA:		printf("ERROR: No hay suficiente memoria disponible o el archivo está corrupto.\n");
                				break;

	}
	printf("\n");
	return;
}

void dimension(tTablero * tablero)
{
	do
		tablero->columnas=getint("Ingrese numero de columnas(mínimo 3).\n");
	while ((tablero->columnas)<3);

	do
		tablero->filas=getint("Ingrese numero de filas(mínimo 3).\n");
	while ((tablero->filas)<3);
}

void cantNiveles(int * cantNiv, int maxNivel)
{
	do
		*cantNiv=getint("Ingrese la cantidad de niveles que quiere jugar(mínimo 2, máximo %d).\n",maxNivel);
	while (*cantNiv<2 || *cantNiv>maxNivel);
}

void imprimeTablero (tTablero * tablero)
{
	int i,j;
	printf("    ");
	for(i=0;i<(tablero->columnas);i++)
               	printf("%-3d ",i); /*Imprime el vector horizontal de numeros*/
	putchar('\n');
	for(i=0;i<(tablero->filas);i++)
	{
	        printf("%-3d ",i); /*Imprime el vector vertical de números*/
		for(j=0;j<(tablero->columnas);j++)
			printf("%c  %c",(tablero->tablero)[i][j],((j==(tablero->columnas)-1)?'\n':' '));

	}
	putchar('\n');
}

int salir(void)
{
	char c;
	do
	{
		printf("Desea guardar la partida?(s/n)\n");
		scanf("%c",&c);
		while((getchar())!='\n');
	}
	while (c!='s' && c!='n');
	if (c=='n')
		return NO;
	if (c=='s')
		return SI;
	return 29;
}
