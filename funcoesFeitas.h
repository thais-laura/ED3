#ifndef __FUNCOES_FEITAS__
#define __FUNCOES_FEITAS__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./funcoesFornecidas.h"

typedef struct{
    int id;
    char nome[41];
    char nome_cient[61];
    int populacao;
    char status[9];
    float localizacao[2];
    int impacto_hum;
} Especie;

void inicializa_especie(Especie *esp);
int le_registro(char *nome_arq, int rrn, int relatorio);
int registra_info(char *nome_arq, int id, int dados[3], int populacao, char *status, int impacto_hum);
int imprime_registro(Especie *esp);
char *imprime_campo(char *campo);
void completa_campo(char* str, int tamanho);
int verifica_id(FILE* arquivo, int id);
int verifica_string(char* str);
int verifica_status(char* status);
int registra_especie(char* nome_arq, int n);

#endif // __FUNCOES_FEITAS__