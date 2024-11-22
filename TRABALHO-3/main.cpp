// Gabriella Castelari Gonçalves - 14755082
// Thaís Laura Anício Andrade - 1460876
// Porcentagem de participação de ambas as alunas foi 100%.

#include <iostream>
#include <string>
#include <stdio.h>
#include "./mensagens.h"
#include "./funcionalidades.h"
#include "./cabecalho.h"
#include "./funcoesArquivo.h"
#include "./funcoesFornecidas.h"
#include "./funcoesGrafo.h"
#include "./structs.h"


int main(){
    int f;
    char nome_arq[50];
    scanf("%d %s", &f, nome_arq);
    if(f == 10){
        funcionalidade10(nome_arq);
    }else if(f == 11){
        int n;
        scanf("%d", &n);
        funcionalidade11(nome_arq, n);
    }else if(f == 12){
        funcionalidade12(nome_arq);
    }
    else if(f == 13){
         funcionalidade13(nome_arq);
    }else if(f == 14){
        int n;
        scanf("%d", &n);
        funcionalidade14(nome_arq, n); 
    }else{
        msg_erro_padrao();
    } 
    return 0;
}