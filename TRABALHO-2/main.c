// Gabriella Castelari Gonçalves - 14755082
// Thaís Laura Anício Andrade - 1460876
// Porcentagem de participação de ambas as alunas foi 100%.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./cabecalho.h"
#include "./cabecalhoArv.h"
#include "./funcionalidades.h"
#include "./funcoesArquivo.h"
#include "./funcoesArvore.h"
#include "./funcoesFornecidas.h"
#include "./mensagens.h"
#include "./structs.h"

int main(){
    int f;
    char nome_arqInd[50],nome_arqDados[50];
    scanf("%d %s %s", &f, nome_arqDados, nome_arqInd);
    if(f == 7){
        funcionalidade7(nome_arqDados, nome_arqInd);
    }else if(f == 8){
        char campo[10], nome[50];
        scanf("%s", campo);
        if(strcmp(campo, "nome") != 0){
            msg_erro_padrao();
            return 0;
        }
        scan_quote_string(nome);
        funcionalidade8(nome_arqDados, nome_arqInd, nome);

    }else if(f == 9){
        int n;
        scanf("%d", &n);
        funcionalidade9(nome_arqDados, nome_arqInd, n);

    }else msg_funcionalidade_inexistente();  
    return 0; 
}