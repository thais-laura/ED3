#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./funcoesFornecidas.h"
#include "./funcoesFeitas.h"
#define TAM 20      //indica o tamanho do nome do arquivo
#define ERRO -1
#define REG 131     //somei todos os bytes dos campos

//float gravado em 2 casas decimais


int main(){
    int comando;
    char nome_arq[TAM], aux[3];

    fgets(aux, 3, stdin);
    if((aux[0]=='1')||(aux[0] == '2')||(aux[0] == '3')||(aux[0] == '4')){
        comando = atoi(aux); 
    }else{
        printf("Entrou no comando: %c\nNao foi 1, 2, 3 ou 4\n", aux[0]);
        printf("Falha no processamento do arquivo\n");
        return 0;
    }
    readline(nome_arq);

    if(comando == 1){
        printf("Entrou no comando: %d\n", comando);
        int n;
        scanf("%d", &n);
        registra_especie(nome_arq, n);

    }else if(comando == 2){
        printf("Entrou no comando: %d\n", comando);

        if(le_registro(nome_arq, 0, 1)!=0)
            printf("Erro ao ler todos os registros - relatorio\n");
        else printf("Deu tudo certo ao ler todos os registros - relatorio\n");

    }else if(comando == 3){
        printf("Entrou no comando: %d\n", comando);

        int rrn;
        scanf("%d", &rrn);
        if(le_registro(nome_arq, rrn, 0)!=0)
            printf("Erro ao ler registro pelo rrn\n");
        else printf("Deu tudo certo ao ler um registro pelo rrn\n");

    }else if(comando == 4){
        int id, qtd_info, dados[3];
        dados[0]=0;
        dados[1]=0;
        dados[2]=0;
        char status[9], instrucao[15];
        int populacao, impacto_hum;
        printf("Entrou no comando: %d\n", comando);
        scanf("%d %d", &id, &qtd_info);                             

        for(int i=0; i< 2* qtd_info; i++){         // utilizar as mesmas verificações que a gabi fez no comando 1
            //ler string e o dado
            scanf("%s", instrucao);
            // verificar se realmente está nulo o campo
            if(strcmp(instrucao,"STATUS") == 0){    //fazer verificao gabi comando 1
                dados[0] = 1;
                scanf("%s", status);
            }
            if(strcmp(instrucao,"POPULATION") == 0){    //fazer verificao gabi comando 1
                dados[1] = 1;
                scanf("%d", &populacao);
            }
            if(strcmp(instrucao,"HUMAN IMPACT") == 0){    //fazer verificao gabi comando 1
                dados[2] = 1;
                scanf("%d", &impacto_hum);
            }
        }

        if(registra_info(nome_arq, id, dados, populacao, status, impacto_hum) !=0)
            printf("Erro ao registrar alguma informação extra\n");
        else printf("Deu tudo certo ao registrar alguma informação extra\n");

    }

    return 0;
}



/*
rb: somente para leitura (o arquivo deve existir)
wb: somente para escrita a partir do início do arquivo (se o arquivo não existir, cria um novo. Se já existir, apaga seu conteúdo antes da escrita)
ab: adiciona a um arquivo. A escrita é feita a partir do final do arquivo (se o arquivo não existir, cria um novo)
r+b: leitura e escrita no início do arquivo.
w+b: leitura e escrita no início do arquivo (apaga o conteúdo se o arquivo existir ou cria um novo arquivo caso contrário)
a+b: abre para leitura no início do arquivo e escrita no seu final (se o arquivo não exisitr, cria um novo)
*/