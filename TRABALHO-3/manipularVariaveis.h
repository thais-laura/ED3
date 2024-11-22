#ifndef MANIPULARVARIAVEIS_H_INCLUDED
#define MANIPULARVARIAVEIS_H_INCLUDED
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Funções para manipular diferentes tipos de dados que compõem os campos do registro
int verifica_float(float a, float b);
char le_char_ou_nulo();
int le_inteiro_ou_nulo();
void le_e_aloca_string(char **campo);
int verifica_stringNula(char *campo);
void grava_str(FILE *arq, char *str);
void le_str(FILE *arq, char **str);

#endif // MANIPULARVARIAVEIS_H_INCLUDED
