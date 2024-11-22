#ifndef STRUCTS_H_INCLUDED
#define STRUCTS_H_INCLUDED

#include <iostream>
#include <stdio.h>
#include <string>
#include <set>

typedef struct registro_dados {
    // Campos fixos
    int populacao;          // Pode ser nulo e repetido
    float tamanho;          // Pode ser nulo e repetido
    char unidadeMedida;     // Pode ser nulo e repetido
    int velocidade;         // Pode ser nulo e repetido
    // Campos variáveis
    char *nome;             // Não pode ser nulo nem repetido
    char *especie;          // Não pode ser nulo nem repetido
    char *habitat;          // Pode ser nulo e repetido
    char *tipo;             // Pode ser nulo e repetido
    char *dieta;            // Não pode ser nulo mas pode ser repetido
    char *alimento;         // Pode ser nulo
    // Campos de controle
    char removido;          // '1' ou '0'
    int encadeamento;       // Caso não esteja na pilha, é dado como -1
} Especie;

typedef struct registro_cabecalho{
    char status;            // Arquivo consistente ('1') ou não ('0')
    int topo;               // Topo da pilha lógica de removidos
    int proxRRN;            // Próximo RRN disponível
    int nroRegRem;          // Quantidade de registros removidos 
    int nroPagDisco;        // Quantidade de páginas de disco
    int qttCompacta;        // Quantidade de compactações feitas
} Cabecalho;

typedef struct aresta{
    std::string nome;       // Nome geral da espécie da presa
    int populacao;          // População da espécie da presa
    // Sobrecarga do operador < para a estrutura set na lista de arestas
    bool operator<(const aresta& outra) const {
        return nome < outra.nome; // Ordena as arestas pelo nome
    }
} Aresta;

typedef struct vertice{
    std::string nome;           // Nome geral da espécie
    std::string especie;        // Nome da espécie
    std::string habitat;        // País onde a espécie vivia
    std::string dieta;          // Tipo de alimentação da espécie
    std::string tipo;           // Categoria da espécie
    int grau_entrada;           // Quantidade de arestas que chegam no vértice  
    int grau_saida;             // Quantidade de arestas que saem do vértice
    int grau;                   // Quantidade total de arestas do vértice
    std::set<Aresta> inicio;    // Lista de arestas que saem do vértice
    // Sobrecarga do operador < para a estrutura set na lista de vértices
    bool operator<(const vertice& outro) const {
        return nome < outro.nome;   // Ordena os vértices pelo nome
    }
    bool operator==(const vertice&  outro) const{
        return nome == outro.nome;
    }
} Vertice;

typedef struct grafo{
    std::set<Vertice> listaVertices;    // Lista de vértices do grafo
    int numVertices;                    // Quantidade de vértices do grafo
} Grafo;

#endif //STRUCTS_H_INCLUDED