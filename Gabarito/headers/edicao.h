#ifndef __EDICAO__
#define __EDICAO__

#include "especies.h"

typedef struct _criterios_edicao CRITERIOS_EDICAO;

/**
 * Realiza a leitura (stdin) de um campo de uma espécie a ser alterada
 *
 * Retorna uma struct de critério de busca com a nova informação
 * Retorna uma struct espécie NULL em caso de erro
 */
CRITERIOS_EDICAO *ler_criterios_edicao();

/**
 * Compara um campo a ser alterado com o respectivo campo no arquivo de dados
 *
 * Retorna verdadeiro caso a informação já esteja cadastrada
 * Retorna falso caso a informação esteja marcada como nulo
 */
bool verificar_dado_ja_cadastrado(ESPECIE *especie, CRITERIOS_EDICAO *criterio_edicao);

/**
 * Reescreve um campo no arquivo de dados de acordo com o que foi pedido pelo usuário.
 * O byteOffset representa a posição de início do registro no arquivo de dados
 */
void editar_campo(CRITERIOS_EDICAO *criterio_edicao, FILE *arquivo, long long int byteOffset);

#endif  // __EDICAO__