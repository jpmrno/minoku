#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define BLOQUE 5
#define TRUE 1
#define FALSE 0

char * leerString(void)
{
	char *string = NULL, *aux;
	char c;
	int dim=0;

	while((c=getchar()) != '\n')
	{
		if(dim%BLOQUE == 0)
		{
			aux=realloc(string, (dim+BLOQUE)*sizeof(*string));
			if(aux == NULL)
			{
				free(string);
				return NULL;
			}
			string=aux;
		}
		string[dim++]=c;
	}
	string[dim]='\0';
	return string;
}

int validarComando(char * comando) {
	char * funcion;
	char * parametros;


}

int main(void) {
	char * comando, * parametros;
	int error;

	do {
		error = FALSE;
		scanf("%6s %8s", comando, parametros)
		if(comando == NULL)
			error = TRUE;
	} while(error && validarComando(comando));

	return 0;
}