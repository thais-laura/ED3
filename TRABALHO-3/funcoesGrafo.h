#ifndef FUNCOESGRAFO_H_INCLUDED
#define FUNCOESGRAFO_H_INCLUDED

#include <iostream>
#include <stdio.h>
#include <string>
#include <set>
#include <list>
#include "./structs.h"


// Inicializar a estrutura do grafo
Grafo inicializa_grafo();
// Realiza a leitura do arquivo binário e cria o grafo a partir dos registros
Grafo cria_grafo(FILE *arq, int proxRRN);
// Acrescenta um novo vértice à lista do grafo durante a leitura do arquivo
void adiciona_verticeEspecie(Grafo &grafo, Especie esp);
// Acrescenta um novo vértice à lista do grafo durante a adição de uma aresta
void adiciona_verticeAresta(Grafo &grafo, Aresta a);
// Acrescenta uma nova aresta à lista do vértice
void adiciona_aresta(Grafo &grafo, Vertice v, Aresta a);
// Impressão de todo o grafo, vértices e arestas
void imprime_grafo(Grafo& grafo);
void imprime_vertice(const Vertice&v);
void imprime_aresta(const Aresta &a);

// Busca todos os predadores de uma presa e imprime seus nomes
void listaPredadores(Grafo &grafo, const std::string &presaNome);

// Função recursiva para encontrar os ciclos
void dfs_qtd(Grafo grafo, Vertice v, std::set<Vertice> &visitados, std::list<Vertice> &pilha, std::set<std::pair<std::string, std::string>> &laco,int &ciclos);
// Função principal para detecção de ciclos: chama dfs (função acima) para realizar a contagem
int conta_ciclos(Grafo& grafo);

// Adaptação da busca de profundidade para a técnica de contagem de componentes conexos
void dfs_compConexo(Grafo &grafo, Vertice v, std::set<Vertice> &visitados, std::list<std::string> &pilha);
// Gera o grafo transposto (muda a direção das arestas)
Grafo transposicao(Grafo grafo);
// Junta as funções sobre componentes conexos e permite a contagem deles em um grafo
int conta_compConexo(Grafo& grafo);
// Verifica se todos os vértices são visitados a partir de um vértice
void dfs_fortemConexo(Grafo grafo, Vertice v, std::set<Vertice> &visitados);
// Retorna verdadeiro se o grafo é fortemente conexo e falso, caso contrário
bool verifica_fortemConexo(Grafo grafo);

// Algoritmo de Dijkstra para encontrar menor caminho entre dois vértices
void processaMenorCaminho(Grafo &grafo, const std::string &predador, const std::string &presa);

#endif //FUNCOESGRAFO_H_INCLUDED