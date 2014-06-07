#ifndef MINOKU_MISC_H
#define MINOKU_MISC_H

// Genera numeros pseudoaleatorios entre [0,1)
double randNormalize(void);

// Genera numeros enteros pseudoaleatorios entre [izq,der]
int randInt(int izq, int der);

// Printea con el estilo \n~~~ <Texto> ~~~\n\n
void printa(const char s[]);

// Printea mensaje segun el error
void printError(int error);

#endif
