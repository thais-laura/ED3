#include <stdio.h>
#include <iostream>
#include <string>
#include <unordered_map>
#include <set>
#include <queue>
#include <vector>
#include <limits>
#include "./funcionalidades.h"
#include "./cabecalho.h"
#include "./mensagens.h"
#include "./funcoesArquivo.h"
#include "./funcoesFornecidas.h"
#include "./funcoesGrafo.h"

/* A funcionalidade 10 cria um grafo a partir da leitura do arquivo binário
e depois imprime todos os vértices em conjunto com suas arestas */
void funcionalidade10(char nome_arq[50]){
    FILE *arq = fopen(nome_arq, "rb");
    Cabecalho cab;
    le_cabecalho(arq, &cab);
    // Se o arquivo estiver inconsistente, o programa deve ser interrompido
    if(cab.status == '0'){      
        msg_erro_padrao();
        fclose(arq);
        return;
    }
    // Caso não haja registros a serem inseridos, o programa deve ser interrompido
    if(cab.proxRRN == cab.nroRegRem || cab.proxRRN == 0){
        msg_registro_inexistente();
        fclose(arq);
        return;
    }
    Grafo grafo;
    grafo = cria_grafo(arq, cab.proxRRN);   // Criação do grafo com base no arquivo binário
    fclose(arq);
    imprime_grafo(grafo);                   // Impressão dos vértices e das arestas
}

void funcionalidade11(char nome_arq[50], int n) {
    FILE *arq = fopen(nome_arq, "rb");
    Cabecalho cab;
    le_cabecalho(arq, &cab);
    // Se o arquivo estiver inconsistente, o programa deve ser interrompido
    if(cab.status == '0'){      
        msg_erro_padrao();
        fclose(arq);
        return;
    }
    // Caso não haja registros a serem inseridos, o programa deve ser interrompido
    if(cab.proxRRN == cab.nroRegRem || cab.proxRRN == 0){
        msg_registro_inexistente();
        fclose(arq);
        return;
    }
    Grafo grafo;
    grafo = cria_grafo(arq, cab.proxRRN);   // Criação do grafo com base no arquivo binário
    fclose(arq);

    char presa[100];
    for (int i = 0; i < n; ++i) {
        scan_quote_string(presa);  // Lê o nome da presa entre aspas
        listaPredadores(grafo, std::string(presa));    // Imprime os predadores da presa
    }
}

void funcionalidade12(char nome_arq[50]){
    FILE *arq = fopen(nome_arq, "rb");
    Cabecalho cab;
    le_cabecalho(arq, &cab);
    // Se o arquivo estiver inconsistente, o programa deve ser interrompido
    if(cab.status == '0'){      
        msg_erro_padrao();
        fclose(arq);
        return;
    }
    // Caso não haja registros a serem inseridos, o programa deve ser interrompido
    if(cab.proxRRN == cab.nroRegRem || cab.proxRRN == 0){
        msg_registro_inexistente();
        fclose(arq);
        return;
    }
    Grafo grafo;
    grafo = cria_grafo(arq, cab.proxRRN);   // Criação do grafo com base no arquivo binário
    fclose(arq);

    int qtd = conta_ciclos(grafo);
    printf("Quantidade de ciclos: %d\n", qtd);
}

void funcionalidade13(char nome_arq[50]){
    FILE *arq = fopen(nome_arq, "rb");
    Cabecalho cab;
    le_cabecalho(arq, &cab);
    // Se o arquivo estiver inconsistente, o programa deve ser interrompido
    if(cab.status == '0'){      
        msg_erro_padrao();
        fclose(arq);
        return;
    }
    // Caso não haja registros a serem inseridos, o programa deve ser interrompido
    if(cab.proxRRN == cab.nroRegRem || cab.proxRRN == 0){
        msg_registro_inexistente();
        fclose(arq);
        return;
    }
    Grafo grafo;
    grafo = cria_grafo(arq, cab.proxRRN);   // Criação do grafo com base no arquivo binário
    fclose(arq);

    bool fortemConexo = verifica_fortemConexo(grafo);
    if(fortemConexo){
        printf("Sim, o grafo é fortemente conexo e possui ");
    }else{
        printf("Não, o grafo não é fortemente conexo e possui ");
    }
    int qtd_compConexo = conta_compConexo(grafo);
    if(qtd_compConexo == 1){
        printf("1 componente.\n");
    }else{
        printf("%d componentes.\n", qtd_compConexo);
    }
}

void funcionalidade14(char *nome_arq, int n) {
    FILE *arq = fopen(nome_arq, "rb");
    Cabecalho cab;
    le_cabecalho(arq, &cab);
    // Se o arquivo estiver inconsistente, o programa deve ser interrompido
    if(cab.status == '0'){      
        msg_erro_padrao();
        fclose(arq);
        return;
    }
    // Caso não haja registros a serem inseridos, o programa deve ser interrompido
    if(cab.proxRRN == cab.nroRegRem || cab.proxRRN == 0){
        msg_registro_inexistente();
        fclose(arq);
        return;
    }
    Grafo grafo;
    grafo = cria_grafo(arq, cab.proxRRN);   // Criação do grafo com base no arquivo binário
    fclose(arq);

    char predador[50], presa[50];
    for (int i = 0; i < n; ++i) {
        scan_quote_string(predador); // Lendo o nome do predador
        scan_quote_string(presa);    // Lendo o nome da presa
        processaMenorCaminho(grafo, std::string(predador), std::string(presa));
    }
}