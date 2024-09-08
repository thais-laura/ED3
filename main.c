#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./funcoesFornecidas.h"
#include "./funcoesFeitas.h"
#define TAM 20      //indica o tamanho do nome do arquivo
#define ERRO -1
#define REG 136     //somei todos os bytes dos campos

//float gravado em 2 casas decimais


int main(){
    
    int comando;
    char nome_arq[TAM], aux[3];

    fgets(aux, 3, stdin);
    if((aux[0]=='1')||(aux[0] == '2')||(aux[0] == '3')||(aux[0] == '4')){
        comando = atoi(aux); 
    }else{
        printf("Falha no processamento do arquivo\n");
        return 0;
    }
    fgets(nome_arq, sizeof(nome_arq), stdin);
    nome_arq[strcspn(nome_arq, "\n")] = 0;

    if(comando == 1){
        int n;
        scanf("%d", &n);
        getchar();
        registra_especie(nome_arq, n);

    }else if(comando == 2){
        le_registro(nome_arq, 0, 1);

    }else if(comando == 3){

        int rrn;
        scanf("%d", &rrn);
        getchar();
        le_registro(nome_arq, rrn, 0);

    }else if(comando == 4){
        int id, qtd_info, populacao = 0, impacto_hum = 0;
        char instrucao[15], status[9] ={};
        int dados[3] = {0, 0, 0};

        scanf("%d %d", &id, &qtd_info);  
        getchar();

        for(int i=0; i< qtd_info; i++){         // utilizar as mesmas verificações que a gabi fez no comando 1
            fgets(instrucao, sizeof(instrucao), stdin);
            instrucao[strcspn(instrucao, "\n")] = 0; 
            
            if(strcmp(instrucao,"POPULATION") == 0){    //fazer verificao gabi comando 1
                dados[0] = 1;
                scanf("%d", &populacao);
                getchar();
            }else if(strcmp(instrucao,"STATUS") == 0){    //fazer verificao gabi comando 1
                dados[1] = 1;
                scanf("%s", status);
                getchar();
                if(verifica_status(status) != 0){
                    printf("Falha no processamento do arquivo\n");
                    dados[1] = 0;
                }
                
            }else if(strcmp(instrucao,"HUMAN IMPACT") == 0){    //fazer verificacao gabi comando 1
                dados[2] = 1;
                scanf("%d", &impacto_hum);
                getchar();
                if (impacto_hum < 1 || impacto_hum > 3) {
                    printf("Falha no processamento do arquivo\n");
                    dados[2] = 0;
                }
            }else{
                printf("Falha no processamento do arquivo\n");
            }
        }
        registra_info(nome_arq, id, dados, populacao, status, impacto_hum);
    }

    return 0;
}