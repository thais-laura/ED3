#include "./cabecalho.h"
#include "./cabecalhoArv.h"
#include "./funcionalidades.h"
#include "./funcoesArquivo.h"
#include "./funcoesArvore.h"
#include "./funcoesFornecidas.h"
#include "./manipularVariaveis.h"
#include "./mensagens.h"
#include "./structs.h"

// Salva as modificações do cabeçalho
void grava_cabecalhoArv(FILE *arq, CabecalhoArv cab){
    if(arq == NULL){            
        msg_arquivo_inexistente();  // Não foi possível manipular o arquivo
        return;
    }
    fseek(arq, 0, SEEK_SET);                    // Desloca o cursor para o início do arquivo
    fwrite(&(cab.status), sizeof(char), 1, arq);
    fwrite(&(cab.noRaiz), sizeof(int), 1, arq);
    fwrite(&(cab.RRNproxNo), sizeof(int), 1, arq);
}

// Lê os campos do cabeçalho
void le_cabecalhoArv(FILE *arq, CabecalhoArv *cab){
    if(arq == NULL){            
        msg_arquivo_inexistente();  // Não foi possível manipular o arquivo
        return;
    }
    fseek(arq, 0, SEEK_SET);            // Desloca o cursor para o início do arquivo
    fread(&(cab->status), sizeof(char), 1, arq);
    fread(&(cab->noRaiz), sizeof(int), 1, arq);
    fread(&(cab->RRNproxNo), sizeof(int), 1, arq);
    char aux[160];
    fread(&aux, sizeof(char), 93 - sizeof(char)-2*sizeof(int), arq);
}

// Imprime os campos do cabeçalho
void imprime_cabecalhoArv(CabecalhoArv cab){
    printf("\nImpressão do cabeçalho\n");
    printf("Status: %c\n", cab.status);
    printf("Nó da Raiz: %d\n", cab.noRaiz);
    printf("Pŕoximo RRN disponível: %d\n", cab.RRNproxNo);
}

// Quando for começar um novo cabeçalho, é necessário inicilizar o cabeçalho em sua forma padrão
void inicia_cabecalhoArv(FILE *arq){
    if(arq == NULL){            
        msg_arquivo_inexistente();  // Não foi possível manipular o arquivo
        return;
    }
    CabecalhoArv cab;
    int tam_cabecalho;
    char c = '$';                       // Caractere lixo 

    cab.status = '1';                   // Arquivo consistente
    cab.noRaiz = -1;                    // Ainda não existe nó raiz da árvore
    cab.RRNproxNo = 0;                    // Ainda não foram adicionados registros

    grava_cabecalhoArv(arq, cab);
    // Termina de completar a primeira página de disco (lixo $)
    tam_cabecalho = ftell(arq);
    for(int i=0; i<(93 - tam_cabecalho); i++){
        fwrite(&c, sizeof(char), 1, arq);
    }
}

// Quando o arquivo é aberto, status é definido como '0' - inconsistente
// Quando finaliza a manipulação do arquivo, status é retornado para '1' - consistente
int atualiza_statusArv(FILE *arq, char status){
    if(arq == NULL){            
        msg_arquivo_inexistente();  // Não foi possível manipular o arquivo
        return -1;
    }
    if(status != '0' && status != '1'){     // Caso tenha algum erro na chamada da função
        msg_erro_padrao();
        return 1;               // Mudança não realizada
    }
    CabecalhoArv cab;
    le_cabecalhoArv(arq, &cab);
    //imprime_cabecalhoArv(cab);
    // O arquivo já está sendo manipulado, então não é possível iniciar outra funcionalidade
    // O programa é finalizado nesses casos
    if(status == '0' && cab.status == '0'){ 
        return 1;               // Mudança não realizada
    }
    cab.status = status;
    grava_cabecalhoArv(arq, cab); 
    return 0;                   // Mudança realizada com sucesso
} 

void atualiza_noRaiz(FILE *arq, int raiz){
    if(arq == NULL){            
        msg_arquivo_inexistente();  // Não foi possível manipular o arquivo
        return;
    }
    CabecalhoArv cab;
    le_cabecalhoArv(arq, &cab);
    if(cab.noRaiz != raiz){
        cab.noRaiz = raiz;
        grava_cabecalhoArv(arq, cab);
    }
}

// Quando há alguma inserção bem-sucedida, o próximo RRN disponível será aumentado
void aumenta_RRNArv(FILE *arq, int qtd_insercao){
    if(arq == NULL){            
        msg_arquivo_inexistente();  // Não foi possível manipular o arquivo
        return;
    }
    if(qtd_insercao <=0){   // Caso tenha algum erro na chamada da função
        msg_erro_padrao(); 
        return;
    }
    CabecalhoArv cab;
    le_cabecalhoArv(arq, &cab);
    cab.RRNproxNo += qtd_insercao; // Atualiza o próximo RRN disponível para uma inserção no final do arquivo
    grava_cabecalhoArv(arq, cab);
}