#ifndef __MENSAGENS__
#define __MENSAGENS__

#include <stdio.h>

/**
 * Imprime uma mensagem de erro
 * dizendo que o rrn é invalido
 */
void msg_rrn_invalido();

/**
 * Imprime uma mensagem de erro
 * dizendo que o SPECIES_ID é inexistente
 */
void msg_species_id_inexistente();

/**
 * Imprime uma mensagem de erro
 * dizendo que o arquivo nao existe
 */
void msg_arquivo_inexistente();

/*
 * Imprime uma mensagem de erro
 * dizendo que algum dado já foi inserido
 */
void msg_dados_ja_cadastrados();

/**
 * Imprime uma mensagem de erro
 * dizendo que a funcionalidade nao foi implementada
 */
void msg_funcionalidade_inexistente();

/**
 * Imprime uma mensagem de erro
 * dizendo falha no processamento do arquivo
 */
void msg_erro_padrao();

#endif  // __MENSAGENS__