#ifndef CABECALHOARV_H_INCLUDED
#define CABECALHOARV_H_INCLUDED
#include <stdio.h>
#include <stdlib.h>
#include "./cabecalho.h"
#include "./funcionalidades.h"
#include "./funcoesArquivo.h"
#include "./funcoesArvore.h"
#include "./funcoesFornecidas.h"
#include "./manipularVariaveis.h"
#include "./mensagens.h"
#include "./structs.h"

// Funções auxiliares para a manipulação do cabeçalho
void grava_cabecalhoArv(FILE *arq, CabecalhoArv cab);
void imprime_cabecalhoArv(CabecalhoArv cab);
void le_cabecalhoArv(FILE *arq, CabecalhoArv *cab);

// Quando for criar um novo arquivo
void inicia_cabecalhoArv(FILE *arq);

// Quando o arquivo for ser alterado, o status deve ser modificado para 0 (verifica se já está como inconsistente)
// Quando estiver finalizando as alterações no arquivo, status deve ser igual a 1 novamente
int atualiza_statusArv(FILE *arq, char status); 

void atualiza_noRaiz(FILE *arq, int raiz);

// Inserção bem-sucedida
void aumenta_RRNArv(FILE *arq, int qtd_insercao);

#endif //CABECALHOARV_H_INCLUDED