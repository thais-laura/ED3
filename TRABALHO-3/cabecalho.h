#ifndef CABECALHO_H_INCLUDED
#define CABECALHO_H_INCLUDED
#include <stdio.h>
#include <stdlib.h>
#include "./structs.h"

// Funções auxiliares para a manipulação do cabeçalho
void grava_cabecalho(FILE *arq, Cabecalho cab);
void imprime_cabecalho(Cabecalho cab);
void le_cabecalho(FILE *arq, Cabecalho *cab);

// Quando for criar um novo arquivo
void inicia_cabecalho(FILE *arq);

// Quando o arquivo for ser alterado, o status deve ser modificado para 0 (verifica se já está como inconsistente)
// Quando estiver finalizando as alterações no arquivo, status deve ser igual a 1 novamente
int atualiza_status(FILE *arq, char status); 

// Remoção bem-sucedida para atualizar o topo da pilha de removidos
// Retorna o RRN que estava no topo para realocação da sequência de removidos
int atualiza_topo(FILE *arq, int RRN_removido);         

// Inserção bem-sucedida
void aumenta_RRN(FILE *arq, int qtd_insercao);

// Compactação bem-sucedida
void diminui_RRN(FILE *arq, int qtd_compactacao);

// Remoção bem-sucedida
void atualiza_remocao(FILE *arq, int qtd_remocao);

// Inserção ou compactação bem-sucedidas
void atualiza_PagDisco(FILE *arq);     

// Compactação bem-sucedida
void atualiza_qttCompacta(FILE *arq, int qtd_compactacao);  

#endif 