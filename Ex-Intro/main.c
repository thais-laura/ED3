#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./funcoesFornecidas.h"
#include "./funcoesFeitas.h"
#define TAM 20      // tamanho máximp do nome do arquivo


int main(){
    
    int comando;                                             // Indica a funcionalidade escolhida
    char nome_arq[TAM], aux[3];                              // Nome do arquivo e variável auxilar para a funcionalidade

    fgets(aux, 3, stdin);
    if((aux[0]=='1')||(aux[0] == '2')||(aux[0] == '3')||(aux[0] == '4')){
        comando = atoi(aux);                                 // Conversão de string para inteiro
    }else{
        printf("Falha no processamento do arquivo\n");
        return 0;
    }
    fgets(nome_arq, sizeof(nome_arq), stdin);
    nome_arq[strcspn(nome_arq, "\n")] = 0;                   // Retirada do '\n'

    if(comando == 1){                                        // Registrar espécie marinha
        int n;                                               // Quantidade "n" de espécies a serem registradas de uma única vez
        scanf("%d", &n);
        getchar();
        registra_especie(nome_arq, n);

    }else if(comando == 2){                                  // Relatório de espécies
        le_registro(nome_arq, 0, 1);

    }else if(comando == 3){                                  // Busca de espécie por RRN
        int rrn;
        scanf("%d", &rrn);
        getchar();
        le_registro(nome_arq, rrn, 0);

    }else if(comando == 4){                                  // Registrar informação de espécie já adicionada
        int id, qtd_info, populacao = 0, impacto_hum = 0;    // Id da espécie, quantidade de informações que serão inseridas e variáveis auxilares para inserção
        char instrucao[15], status[9] ={};                   // Indica qual campo será atualizado e variável auxiliar para inserção
        int dados[3] = {0, 0, 0};                            // Vetor que indica qual das informações serão inseridas (população - 0, status - 1, impacto humano - 2)

        scanf("%d %d", &id, &qtd_info);  
        getchar();
        if(id == 0){
            printf("Espécie não encontrada\n");
            return 0;
        }

        for(int i=0; i < qtd_info; i++){         
            fgets(instrucao, sizeof(instrucao), stdin);
            instrucao[strcspn(instrucao, "\n")] = 0;         // Retirada do '\n'
            
            if(strcmp(instrucao,"POPULATION") == 0){         
                dados[0] = 1;                                // Indica tentativa de atualização da população
                scanf("%d", &populacao);
                getchar();
                
            }else if(strcmp(instrucao,"STATUS") == 0){      
                dados[1] = 1;                                // Indica tentativa de atualização do status
                scanf("%s", status);
                getchar();
                
                
            }else if(strcmp(instrucao,"HUMAN IMPACT") == 0){    
                dados[2] = 1;                                // Indica a tentativa de atualização do impacto humano
                scanf("%d", &impacto_hum);
                getchar();
                
            }else{
                printf("Falha no processamento do arquivo\n");
            }
        }
        registra_info(nome_arq, id, dados, populacao, status, impacto_hum);
    }
    return 0;
}