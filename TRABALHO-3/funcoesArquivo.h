#ifndef FUNCOESARQUIVO_H_INCLUDED
#define FUNCOESARQUIVO_H_INCLUDED
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./structs.h"

// Funções que lidam com os registros nos arquivos binário e CSV
void escreve_registro(FILE *bin, Especie *reg);
void le_campoCSV(char **destino, char **linha);
int le_camposFixos(FILE *arq, Especie *esp);
int le_camposVariaveis(FILE *arq, Especie *esp);
void imprime_registro(Especie esp);
void remover_registro(FILE *arq, int rrn);

int busca_populacao_velocidade(FILE *arq, int rem_imp, int campo, int valor, int proxRRN);
int busca_tamanho(FILE *arq, int rem_imp, float valor,  int proxRRN);
int busca_unidadeMedida(FILE *arq, int rem_imp, char valor,  int proxRRN);
int busca_variavelUnica(FILE *arq, int rem_imp, char *valor,  int proxRRN);
int busca_variavelNaoUnica(FILE *arq, int rem_imp, int campo, char *valor,  int proxRRN);


#endif //FUNCOESARQUIVO_H_INCLUDED