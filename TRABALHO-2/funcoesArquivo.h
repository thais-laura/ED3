#ifndef FUNCOESARQUIVO_H_INCLUDED
#define FUNCOESARQUIVO_H_INCLUDED
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./structs.h"

// Funções que lidam com os registros nos arquivos binário e CSV
void escreve_registro(FILE *bin, Especie *reg);
int le_camposFixos(FILE *arq, Especie *esp);
int le_camposVariaveis(FILE *arq, Especie *esp);
void imprime_registro(Especie esp);
void imprime_indice();
//void imprimeNoParaArquivo(No no, FILE *arquivo);

#endif //FUNCOESARQUIVO_H_INCLUDED