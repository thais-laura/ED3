#ifndef __ESPECIES__
#define __ESPECIES__

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "funcoesFornecidas.h"
#include "mensagens.h"

#define TAM_ESPECIE 131

#define TAM_NOME 41
#define TAM_NOME_CIENTIFICO 61
#define TAM_STATUS 9

typedef struct _especie {                         // 131 bytes
    int species_id;                               // 4 bytes
    char name[TAM_NOME];                          // 41 bytes
    char scientific_name[TAM_NOME_CIENTIFICO];    // 61 bytes
    int population;                               // 4 bytes                       ... pode ser null (recebido como 0)
    char status[TAM_STATUS];                      // 9 bytes                       ... pode ser null (recebido como NULO)
    float location[2];                            // 8 bytes (2 floats de 4 bytes)
    int human_impact;                             // 4 bytes                       ... pode ser null (recebido como 0)
} ESPECIE;

/**
 * Desaloca a estrutura espécie
 */
void especie_apagar(ESPECIE **especie);

/**
 * Lê os dados de uma espécie do stdin
 */
ESPECIE* especie_ler();

/**
 * Imprime os dados de uma espécie no stdout,
 * de acordo com a formatação exigida
 */
void especie_imprimir(ESPECIE *especie);

/**
 * Le os dados de uma espécie de um arquivo binario
 *
 * Retorna NULL se houver um erro de alocacao
 * ou se tiver chegado ao final do arquivo ou
 * se houver algum erro de leitura do arquivo
 */
ESPECIE *especie_ler_arquivo(FILE *arquivo);

/**
 * Função auxiliar que escreve num arquivo uma string com tamanho limite
 * 
 * Preenche o que sobrar de tam com lixo (no nosso caso, '$')
 */
bool _arquivo_escrever_string(FILE *arquivo, char *str, int tam);

/**
 * Escreve os dados de uma espécie em um arquivo binario
 *
 * Retorna se foi possivel escrever no arquivo
 */
bool especie_escrever_arquivo(ESPECIE *especie, FILE *arquivo);

/**
 * Realiza busca sequencial no arquivo de dados para encontrar um dado id_species
 * Marca a variável encontrado a depender da presença da espécie no arquivo
 * Marca a variável byteOffset com o início do registro no arquivo, caso esteja lá
 * 
 * Retorna NULL caso a espécie desejada não esteja no arquivo de dados
 * Retorna a struct espécie com todos os dados da espécie buscada caso esteja no arquivo
 */
ESPECIE *buscar_especie(int idEspecie, FILE *arquivo, bool *encontrado, long long int *byteOffset);

#endif  // __ESPECIES__
