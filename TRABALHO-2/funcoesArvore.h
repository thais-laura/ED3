#ifndef FUNCOESARVORE_H_INCLUDED
#define FUNCOESARVORE_H_INCLUDED
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./structs.h"

void imprimeNo(No no);
int verifica_chave(int qtd, long int *campos, long int chave, int *pos_chave);
void leNoArv(FILE *arq, No *no);
int encontra_subarvore(No no, long int chave);
int buscaArv(FILE *arq, int long chave, long int *byteoffset, int *RRN_pagDisco, int *RRN_pai);
int verifica_chave(int qtd, long int *campos, long int chave, int *pos_chave);
void imprimeNoParaArquivo(No no);
void imprime_indice();
void criaNo(FILE *arq, No *novoNo, CabecalhoArv *cabecalho);
void gravaNoArvB(FILE *arq, No no);
void cria_indice(FILE *arqDados, FILE *arqInd, Cabecalho cabDados, CabecalhoArv cabArv);
void insereNo(FILE *arq, long int chave, long int byteoffset, CabecalhoArv *cabecalho);
void split(FILE *arq, No no, long int *chaves, long int *byteoffsets, int *ponteiros, CabecalhoArv *cabecalho, int RRN_pai);

#endif //FUNCOESARVORE_H_INCLUDED