// Gabriella Castelari Gonçalves - 14755082
// Thaís Laura Anício Andrade - 1460876
// Porcentagem de participação de ambas as alunas foi 100%.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./cabecalho.h"
#include "./funcionalidades.h"
#include "./funcoesArquivo.h"
#include "./funcoesFornecidas.h"
#include "./mensagens.h"
#include "./structs.h"

int main(){
    int f, n;
    char nome_bin[50];
    scanf("%d", &f);

    if(f == 1){
        char nome_csv[50];
        scanf("%s %s", nome_csv, nome_bin);
        funcionalidade1(nome_csv, nome_bin);

    }else if(f == 2){
        scanf("%s", nome_bin);
        funcionalidade2(nome_bin);

    }else if(f == 3){
        int n;
        scanf("%s %d", nome_bin, &n);
        funcionalidade3(nome_bin, n);

    }else if(f == 4){
        int n;
        scanf("%s %d", nome_bin, &n);
        funcionalidade4(nome_bin, n);
        
    }else if(f == 5){
        int n;
        scanf("%s %d", nome_bin, &n);
        funcionalidade5(nome_bin, n);

    }else if(f == 6){
        scanf("%s", nome_bin);
        funcionalidade6(nome_bin);

    }else msg_funcionalidade_inexistente();
    
    return 0;
}
