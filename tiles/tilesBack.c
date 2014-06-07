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
///////////////////DECLARACIONES/////////////////////
/////////////////////////////////////////////////////
static void jugadaEliminarRec(int coorFil, int coorCol, int mataux[][4], tTablero * tablero, char color,int *azulejosBorrados); //elimina las celdas contiguas del mismo color que las elegidas
static void jugadaHilera (int coorFil, tTablero * tablero, int * azulejosBorrados, dispJugadas * bonuses); //elimina la hilera elegida
static void jugadaColumna (int coorCol, tTablero * tablero, int * azulejosBorrados, dispJugadas * bonuses); // elimina la columna elegida
static void jugadaMartillo (int coorFil, int coorCol, tTablero * tablero, int *azulejosBorrados, dispJugadas * bonuses); //realiza la jugada martillo
static int jugadaPosible(tTablero * tablero); //verifica si hay jugada eliminar posible
static int deshacerPreventivo(tTablero * tablero, tTablero * undo); //guarda el tablero en una estructura auxiliar para poder realizar el undo
static int valeEliminar (int coorFil, int coorCol, tTablero * tablero); //verifica que la jugada eliminar sea posible
static int valeHilera(int coorFil, tTablero * tablero, dispJugadas * bonuses); //verifica que la jugada hilera sea posible
static int valeColumna (int coorCol, tTablero * tablero, dispJugadas * bonuses); //verifica que la jugada columna sea posible
static int valeMartillo (int coorFil, int coorCol, tTablero * tablero, dispJugadas * bonuses); //verifica que la jugada martillo sea posible
static int cargarBitacora(char * nombre,FILE ** bitacora);
static int guardarBitacora(char * nombre,FILE ** bitacora);
//////////////////////////////////////////////////////
//////////////////////FUNCIONES///////////////////////
//////////////////////////////////////////////////////
void liberaMem(tTablero*tablero)
{
	int i;
	for(i=0;i<tablero->filas;i++)
		free(tablero->tablero[i]);
	free(tablero->tablero);

}

char ** creaMatriz (tTablero * tablero)
{
	int i;
	(tablero->tablero)=calloc((tablero->filas), (tablero->filas)*sizeof(char*));
	if (tablero->tablero==NULL)
	{
		free(tablero->tablero);
		return ERRORMALLOC;
	}
	else
		for (i=0;i<tablero->filas;i++)
		{
			*(tablero->tablero+i)=calloc(tablero->columnas, (tablero->columnas)*sizeof(char));
			if (*(tablero->tablero+i)==NULL)
			{
				liberaMem(tablero);
				return ERRORMALLOC;
			}
		}
		return tablero->tablero;
}


int maxNivel(tTablero * tablero)
{
 if (((tablero->filas)*(tablero->columnas))/2<8)
	return (((tablero->filas)*(tablero->columnas))/2);
 return 8;
}


void creaTablero(int nivel, tTablero * tablero)
{
	int i,j;
	int cantColores=nivel+1;
	srand (time(NULL));

	for(i=0;i<tablero->filas;i++)
   	 {
        	for(j=0;j<tablero->columnas;j++)
            			(tablero->tablero)[i][j]=(rand()%cantColores)+'A';

         }


}

static void jugadaEliminarRec(int coorFil, int coorCol, int mataux[][4], tTablero * tablero, char color,int *azulejosBorrados)/*Recibe el ASCII del elemento y lo compara con los elementos de alrededor*/
{
int k, j=0, i=1;
if(color!=tablero->tablero[coorFil][coorCol])
	{
	return;
	}
tablero->tablero[coorFil][coorCol]=' ';
(*azulejosBorrados)++;
for(k=1;k<5;k++)
	{
	if(!(coorFil+i>=tablero->filas || coorCol+j>=tablero->columnas || coorCol+j<0 || coorFil+i<0))
        jugadaEliminarRec(coorFil+i, coorCol+j, mataux, tablero, color,azulejosBorrados);
    i=mataux[0][k];
	j=mataux[1][k];
	}
}


static int valeEliminar (int coorFil, int coorCol, tTablero * tablero)
{
	int valeJugada=0;
	int k, j=0, i=1, mataux[2][4]={{1,-1,0,0},{0,0,-1,1}}; //la matriz es para combinar los incrementos de i y de j en el for para que compare para arriba, para la derecha, para la izquierda y para abajo
	if(coorFil>=tablero->filas || coorCol>=tablero->columnas || coorCol<0 || coorFil<0)
		return ERROR_FUERA_DE_RANGO;
	if(tablero->tablero[coorFil][coorCol]==' ')
		return ERROR_CELDA_VACIA;
	for(k=1;k<5 && valeJugada==0;k++)
	{
		if(!(coorFil+i>=tablero->filas || coorCol+j>=tablero->columnas || coorCol+j<0 || coorFil+i<0))
		{
			valeJugada=(tablero->tablero[coorFil+i][coorCol+j]==tablero->tablero[coorFil][coorCol]);
		}
		i=mataux[0][k];
		j=mataux[1][k];
	}
	if (valeJugada==0)
		return JUGADA_INVALIDA;
	return 1;
}

static void jugadaHilera (int coorFil, tTablero * tablero, int * azulejosBorrados, dispJugadas * bonuses)
{
	int i;
	for(i=0;i<tablero->columnas;i++)
	{
		if(tablero->tablero[coorFil][i]!=' ') //para que los azulejos borrados sean la cantidad correcta
		{	tablero->tablero[coorFil][i]=' ';
			(*azulejosBorrados)++;
		}
	}
	bonuses->hileras--;
}

static int valeHilera(int coorFil, tTablero * tablero, dispJugadas * bonuses)
{
	int i;
	if(bonuses->hileras==0)
		return ERROR_FALTA_HILERA;
	if(coorFil>=tablero->filas || coorFil <0)
		return ERROR_FUERA_DE_RANGO;

	for (i=0;i<tablero->columnas;i++)
    {
		if (tablero->tablero[coorFil][i]!=' ') //si hay alguna celda en la fila que no sea vacia, retorne 1
			return 1;
    }
	return ERROR_FILA_VACIA;
}

static void jugadaColumna (int coorCol, tTablero * tablero, int * azulejosBorrados, dispJugadas * bonuses)
{
	int i;
	for(i=0;i<tablero->filas;i++)
	{
		if(tablero->tablero[i][coorCol]!=' ') //para que los azulejos borrados sean la cantidad correcta
		{	tablero->tablero[i][coorCol]=' ';
			(*azulejosBorrados)++;
		}
	}
	bonuses->columnas--;

}

static int valeColumna (int coorCol, tTablero * tablero, dispJugadas * bonuses)
{
	int i;
	if(bonuses->columnas==0)
		return ERROR_FALTA_COLUMNA;
	if(coorCol>=tablero->columnas || coorCol <0)
		return ERROR_FUERA_DE_RANGO;
	for (i=0;i<tablero->filas;i++)
		{
		if (tablero->tablero[i][coorCol]!=' ')//si hay alguna celda en la columna que no sea vacia, retorne 1
			return 1;
		}
	return ERROR_COLUMNA_VACIA;
}



static void jugadaMartillo (int coorFil, int coorCol, tTablero * tablero, int *azulejosBorrados, dispJugadas * bonuses)
{
	int i,j;
	for(i=-1;i<2;i++)
	{
		for(j=-1;j<2;j++)
		{
			if(coorFil+i<tablero->filas && coorCol+j<tablero->columnas && coorCol+j>=0 && coorFil+i>=0) //para que no trate de eliminar fuera del tablero (ejemplo posicion -1,4)
			{
				if(tablero->tablero[coorFil+i][coorCol+j]!=' ')
				{	tablero->tablero[coorFil+i][coorCol+j]=' ';
					(*azulejosBorrados)++;
				}
			}
		}
	}
	bonuses->martillos--;

}

static int valeMartillo (int coorFil, int coorCol, tTablero * tablero, dispJugadas * bonuses)
{
	if(bonuses->martillos==0)
		return ERROR_FALTA_MARTILLO;
	if(coorFil>=tablero->filas || coorCol>=tablero->columnas || coorCol<0 || coorFil<0)
        	return ERROR_FUERA_DE_RANGO;
	if(tablero->tablero[coorFil][coorCol]==' ')
        	return ERROR_CELDA_VACIA;
        return 1;
}
int gravedad (tTablero * tablero)
{
	int i,j,k=0; 						//k va a ser una variable auxiliar para guardar la hilera en la que esté la primera celda vacia
	for(j=0;j<tablero->columnas;j++)	//ya que no se podria usar la i porque se pisaria
		for(i=tablero->filas-1,k=tablero->filas-1;i>=0;i--)
		{
			if(tablero->tablero[k][j]!=' ')
				k--;								//cuando no es vacia decrementa porque empieza de la ultima hilera
			if(i<k && tablero->tablero[i][j]!=' ')
			{
				tablero->tablero[k][j]=tablero->tablero[i][j]; 	//aqui se cambia la primera letra encontrada despues de celdas vacias
				tablero->tablero[i][j]=' ';						//por el lugar que habia guardado k
			}
		}
	return 1;
}


int gravedadLateral (tTablero * tablero)
{
	int i,j,k=0;						//mismo razonamiento que en la funcion gravedad solo que aplicado a las columnas
	for(j=0;j<tablero->columnas;j++)
	{
		if(tablero->tablero[tablero->filas-1][k]!=' ')
			k++;
		if(tablero->tablero[tablero->filas-1][j]!=' ' && j>k )
		{
			for (i=tablero->filas-1;i>=0 && tablero->tablero[i][j]!=' ';i--)
			{
				tablero->tablero[i][k]=tablero->tablero[i][j];
				tablero->tablero[i][j]=' ';
			}
		}
	}
	return 1;
}



int finDeNivel (tTablero * tablero, dispJugadas * bonuses)
{
	if (tablero->tablero[tablero->filas-1][0]==' ')
		return GANO_NIVEL;
	if (bonuses->hileras==0 && bonuses->columnas==0 && bonuses->martillos==0 && !jugadaPosible(tablero))
		return PERDIO_NIVEL;
	return SIGA_JUGANDO;
}

static int jugadaPosible(tTablero * tablero)
{
	int i,j;
	for (i=tablero->filas-1;i>=0;i--)
	{
		for (j=0;j<tablero->columnas;j++)
		{
			if ((i-1>=0 && tablero->tablero[i][j]!=' ' && tablero->tablero[i][j]==tablero->tablero[i-1][j]) || (j+1<tablero->columnas && tablero->tablero[i][j]!=' ' && tablero->tablero[i][j]==tablero->tablero[i][j+1]))
				return 1;		//verifica para todo el tablero si hay 2 celdas contiguas que sean de la misma letra
		}
	}
	return 0;
}

int puntuacion(tTablero * tablero, int * azulejosBorrados)
{
	double maxAzu;
	maxAzu=tablero->filas*tablero->columnas;
	if((double)*azulejosBorrados==1)
		return 1;
	if((double)*azulejosBorrados<(maxAzu*0.3))
		return *azulejosBorrados*2;
	if((double)*azulejosBorrados<(maxAzu*0.6))
		return *azulejosBorrados*3;
	return *azulejosBorrados*4;
}

int validaJugadaYEjecuta(char * orden,int coorFil,int coorCol, tTablero * tablero,int * azulejosBorrados, dispJugadas * bonuses, tTablero * undo, guardarCargar * guaCar, FILE ** bitacora)
{
	char barraN,espacio; 						//estos chars son para verificar que entre la orden y las coordenadas(en las jugadas que requieran coordenadas)
	char * archGrabado;							//haya un espacio y no esten pegados(m4,2 por ejemplo)
	int error;
	int jugada;
	int mataux[2][4]={{1,-1,0,0},{0,0,-1,1}};
	static int flagUndo=-1;						//para que no se hagan 2 undos seguidos(inicializado en -1 para que no haya problemas)

	switch (*orden)
	{
		case 'e':	if (sscanf(orden,"e%c%d,%d%c",&espacio,&coorFil,&coorCol,&barraN)==3 && espacio==' ') 	//si despues de la ultima coordenada hay otro caracter entonces da invalido porque
				{																							//el scanf devolveria 4 (mismo criterio en el resto del switch)
					if ((error=valeEliminar (coorFil, coorCol, tablero))==1)
					{
						flagUndo=deshacerPreventivo(tablero,undo); //si la jugada es posible entonces guarda el tablero para un posible undo(mismo criterio en el resto de los if)
						jugadaEliminarRec (coorFil, coorCol,mataux, tablero,tablero->tablero[coorFil][coorCol],azulejosBorrados);
					}
					jugada=ELIMINAR;
				}
				else
					error=INVALIDO;
				break;
		case 'm':	if (sscanf(orden,"m%c%d,%d%c",&espacio,&coorFil,&coorCol,&barraN)==3 && espacio==' ')
				{
					if((error=valeMartillo ( coorFil, coorCol, tablero, bonuses))==1)
					{
						flagUndo=deshacerPreventivo(tablero,undo);
						jugadaMartillo ( coorFil, coorCol, tablero, azulejosBorrados, bonuses);
					}
					jugada=MARTILLO;

				}
				else
					error=INVALIDO;
				break;
		case 'h':	if (sscanf(orden,"h%c%d%c",&espacio,&coorFil,&barraN)==2 && espacio==' ')
				{
					if((error=valeHilera( coorFil, tablero, bonuses))==1)
					{
						flagUndo=deshacerPreventivo(tablero,undo);
						jugadaHilera ( coorFil, tablero, azulejosBorrados, bonuses);
					}
					jugada=HILERA;
				}
				else
					error=INVALIDO;
				break;
		case 'c':	if (sscanf(orden,"c%c%d%c",&espacio,&coorCol,&barraN)==2 && espacio==' ')
				{
					if((error=valeColumna ( coorCol, tablero, bonuses))==1)
					{
						flagUndo=deshacerPreventivo(tablero,undo);
						jugadaColumna ( coorCol, tablero, azulejosBorrados, bonuses);
					}
					jugada=COLUMNA;
				}
				else
					error=INVALIDO;
				break;
		case 'u':	if (!strcmp(orden,"undo"))
				{
					if (flagUndo==0)
					{
						flagUndo=1;
						jugada=UNDO;
						error=1;
					}
					else
						error=UNDO_REPETIDO;
				}
				else
					error=INVALIDO;
				break;
		case 'q':	if (!strcmp(orden,"quit"))
				{	jugada=QUIT;
					error=1;
				}
				else
					error=INVALIDO;
				break;
		case 's':	if (sscanf(orden,"save%c%c",&espacio,&barraN)==2 && espacio==' ' && barraN!=' ')
				{
					archGrabado=malloc(strlen(orden+4));	//crea espacio para el string con el nombre del archivo
					if(archGrabado!=NULL)
						strcpy(archGrabado,orden+5);
					error=guardarPartida(archGrabado, guaCar, tablero, bonuses, bitacora);
					jugada=SAVE;
					if(guaCar->bitacora==0)
						free(archGrabado);
				}
				else
					error=INVALIDO;
				break;
		default:	error=INVALIDO;
	}
	if (error!=1)
		return error;
	return jugada;
}

void cantBonuses(dispJugadas * bonuses, int puntNivel, tTablero * tablero)
{
	int azulejos;
	azulejos=tablero->columnas*tablero->filas;

	if(puntNivel>=(3*azulejos))
		bonuses->martillos++;
	if(puntNivel>=(2*azulejos))
		bonuses->columnas++;
	if(puntNivel>=azulejos)
		bonuses->hileras++;
}

int bitacoraTablero(tTablero * tablero, FILE * bitacora)
{
	int i,j;
	for(i=0;i<(tablero->filas);i++)
	{
		for(j=0;j<(tablero->columnas);j++)
			fputc(((tablero->tablero[i][j]==' ')?'0':tablero->tablero[i][j]), bitacora); //si en el tablero hay una celda vacia(' ') la guarda como 0
		fputc('\n',bitacora);
	}
	return 1;
}

int bitacoraMovimientos(char * orden, FILE * bitacora, int puntuacion, int contador)
{

	fprintf(bitacora,"%d: %s; %d\n",contador,orden,puntuacion);
	return 1;
}

int deshacer(tTablero * tablero, tTablero * undo,dispJugadas * bonuses,int jugada)
{
	int i,j;
	for(i=0;i<tablero->filas;i++)
	{
		for(j=0;j<tablero->columnas;j++)
			(tablero->tablero)[i][j]=(undo->tablero)[i][j]; //realiza el cambio al tablero undo guardado aparte
	}
	switch (jugada)		//devuelve la jugada si se uso una jugada especial
	{
		case MARTILLO:	bonuses->martillos++;
				break;
		case HILERA:	bonuses->hileras++;
				break;
		case COLUMNA:	bonuses->columnas++;
				break;
	}
	return 1;
}

static int deshacerPreventivo(tTablero * tablero, tTablero * undo)
{
	int i,j;
	for(i=0;i<tablero->filas;i++)
	{
		for(j=0;j<tablero->columnas;j++)
			(undo->tablero)[i][j]=(tablero->tablero)[i][j];
	}
	return 0;
}


int guardarPartida(char * nombre, guardarCargar * guaCar, tTablero * tablero, dispJugadas * bonuses, FILE ** bitacora)
{
	int i, j;
	char k='0';
	FILE * guardar;
	guardar=fopen(nombre,"wb");
	if (guardar==NULL)
		return NOMBRE_INCORRECTO;
	if (guaCar->bitacora==1)
			guardarBitacora(nombre, bitacora);
	fwrite(tablero,sizeof(int),2,guardar);
	fwrite(guaCar,sizeof(guardarCargar),1,guardar);
	fwrite(bonuses,sizeof(dispJugadas),1,guardar);
	for (i=0; i<tablero->filas;i++)
	{
		for(j=0;j<tablero->columnas;j++)
			fwrite(((tablero->tablero[i][j])==' '?&k:&(tablero->tablero[i][j])),sizeof(char),1,guardar);
	}
	fclose(guardar);
	return 1;
}

int cargarPartida(char * nombre, guardarCargar * guaCar, tTablero * tablero, dispJugadas * bonuses,FILE ** bitacora)
{
	char **pperror;
	char c,cant=0;
	int cantidad=0, i, j, error;
	FILE * guardar;
	guardar=fopen(nombre,"rb");
	if (guardar==NULL)
	{
		free(nombre);	
		return NO_EXISTE;
	}
	cantidad=fread(tablero,sizeof(int),2,guardar);
	cantidad+=fread(guaCar,sizeof(guardarCargar),1,guardar);
	if (guaCar->bitacora==1)
	{
		error=cargarBitacora(nombre,bitacora);
		if(error!=1)
			return error;
	}
	cantidad+=fread(bonuses,sizeof(dispJugadas),1,guardar);
	pperror=creaMatriz(tablero);
	if(pperror==ERRORMALLOC)
	{
		fclose(guardar);
		free(nombre);
		return ERROR_SIN_MEMORIA;
	}
	for (i=0; i<tablero->filas;i++)
	{
		for(j=0;j<tablero->columnas;j++)
		{
			cantidad+=fread(&(tablero->tablero[i][j]),sizeof(char),1,guardar);
			if (tablero->tablero[i][j]=='0')
				tablero->tablero[i][j]=' ';
		}
	}
	cant=fread(&c,sizeof(char),1,guardar);   //esto valida que no queden mÃ¡s bytes en el archivo
	fclose(guardar);
	if(cantidad!=4+(tablero->filas*tablero->columnas) || cant==1)	//la cantidad de elementos que leyo tiene que ser igual a [4+(tabl...] para no falten datos
		return ERROR_CORRUPCION;									//o bien la cantidad de caracteres de mas debe ser 0 para que el archivo sea correcto
	return 1;
}

static int guardarBitacora(char * nombre,FILE ** bitacora)
{
	char *aux=NULL;
	char buffer[256];
	FILE * bitacoraGuardada;
	aux=realloc(nombre, strlen(nombre)+5);
	if (aux!=NULL)
	{
		nombre=aux;
		strcat (nombre, ".txt");
		bitacoraGuardada = fopen(nombre, "wt");
		fclose(*bitacora);
		*bitacora=fopen ("bitacora.txt", "rt");
		while (fgets(buffer, 256, *bitacora))	//copia de la bitacora original a la que se va a guardar de a 256 caracteres
			fputs (buffer, bitacoraGuardada);
		fclose(*bitacora);
		*bitacora=fopen("bitacora.txt", "at");
		fclose(bitacoraGuardada);
		free(nombre);
 		return 1;
	}
	free(nombre);
	return ERROR_SIN_MEMORIA;
}
static int cargarBitacora(char * nombre,FILE ** bitacora)
{
	char *aux=NULL;
	char buffer[256];
	FILE * bitacoraGuardada;
	aux=realloc(nombre, strlen(nombre)+5);
	if (aux!=NULL)
	{
		nombre=aux;
		strcat(nombre,".txt");
		bitacoraGuardada=fopen(nombre,"rt");
		if(bitacoraGuardada==NULL)
		{
			free(nombre);
			return NO_EXISTE;
		}
		*bitacora=fopen("bitacora.txt","wt");
		while (fgets(buffer,256,bitacoraGuardada))	//igual que en guardarBitacora pero al reves
			fputs(buffer,*bitacora);
		fclose(bitacoraGuardada);
		free(nombre);
		return 1;
	}
	free(nombre);
	return ERROR_SIN_MEMORIA;
}
















