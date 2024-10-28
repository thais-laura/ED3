#include "./cabecalho.h"
#include "./mensagens.h"

// Salva as modificações do cabeçalho
void grava_cabecalho(FILE *arq, Cabecalho cab){
    if(arq == NULL){            
        msg_arquivo_inexistente();  // Não foi possível manipular o arquivo
        return;
    }
    fseek(arq, 0, SEEK_SET);                    // Desloca o cursor para o início do arquivo
    fwrite(&(cab.status), sizeof(char), 1, arq);
    fwrite(&(cab.topo), sizeof(int), 1, arq);
    fwrite(&(cab.proxRRN), sizeof(int), 1, arq);
    fwrite(&(cab.nroRegRem), sizeof(int), 1, arq);
    fwrite(&(cab.nroPagDisco), sizeof(int), 1, arq);
    fwrite(&(cab.qttCompacta), sizeof(int), 1, arq);
}

// Lê os campos do cabeçalho
void le_cabecalho(FILE *arq, Cabecalho *cab){
    if(arq == NULL){            
        msg_arquivo_inexistente();  // Não foi possível manipular o arquivo
        return;
    }
    fseek(arq, 0, SEEK_SET);            // Desloca o cursor para o início do arquivo
    fread(&(cab->status), sizeof(char), 1, arq);
    fread(&(cab->topo), sizeof(int), 1, arq);
    fread(&(cab->proxRRN), sizeof(int), 1, arq);
    fread(&(cab->nroRegRem), sizeof(int), 1, arq);
    fread(&(cab->nroPagDisco), sizeof(int), 1, arq);
    fread(&(cab->qttCompacta), sizeof(int), 1, arq);
}

// Imprime os campos do cabeçalho
void imprime_cabecalho(Cabecalho cab){
    printf("\nImpressão do cabeçalho\n");
    printf("Status: %c\n", cab.status);
    printf("Topo: %d\n", cab.topo);
    printf("Pŕoximo RRN disponível: %d\n", cab.proxRRN);
    printf("Número de registros removidos: %d\n", cab.nroRegRem);
    printf("Número de páginas de disco: %d\n", cab.nroPagDisco);
    printf("Quantidade de compactações: %d\n", cab.qttCompacta);
}

// Quando for começar um novo cabeçalho, é necessário inicilizar o cabeçalho em sua forma padrão
void inicia_cabecalho(FILE *arq){
    if(arq == NULL){            
        msg_arquivo_inexistente();  // Não foi possível manipular o arquivo
        return;
    }
    Cabecalho cab;
    int tam_cabecalho;
    char c = '$';                       // Caractere lixo 

    cab.status = '1';                   // Arquivo consistente
    cab.topo = -1;                      // A pilha de registros removidos é vazia
    cab.proxRRN = 0;                    // Ainda não foram adicionados registros
    cab.nroRegRem = 0;                  // Ainda não há registros removidos
    cab.nroPagDisco = 0;                // Após alguma funcionalidade, a quantidade é atualizada
    cab.qttCompacta = 0;                // Ainda não houve compactações

    grava_cabecalho(arq, cab);
    // Termina de completar a primeira página de disco (lixo $)
    tam_cabecalho = ftell(arq);
    for(int i=0; i<(1600 - tam_cabecalho); i++){
        fwrite(&c, sizeof(char), 1, arq);
    }
}


// Quando o arquivo é aberto, status é definido como '0' - inconsistente
// Quando finaliza a manipulação do arquivo, status é retornado para '1' - consistente
int atualiza_status(FILE *arq, char status){
    if(arq == NULL){            
        msg_arquivo_inexistente();  // Não foi possível manipular o arquivo
        return -1;
    }
    if(status != '0' && status != '1'){     // Caso tenha algum erro na chamada da função
        msg_erro_padrao();
        return 1;               // Mudança não realizada
    }
    Cabecalho cab;
    le_cabecalho(arq, &cab);
    // O arquivo já está sendo manipulado, então não é possível iniciar outra funcionalidade
    // O programa é finalizado nesses casos
    if(status == '0' && cab.status == '0'){ 
        msg_erro_padrao();
        return 1;               // Mudança não realizada
    }
    cab.status = status;
    grava_cabecalho(arq, cab); 
    return 0;                   // Mudança realizada com sucesso
} 

// É preciso gravar no cabeçalho o RRN do último elemento removido para construir uma pilha lógica
// Essa função retorna o penúltimo RRN de registro removido para ser inserido no encadeamento do registro
int atualiza_topo(FILE *arq, int RRN_removido){
    if(arq == NULL){            
        msg_arquivo_inexistente();  // Não foi possível manipular o arquivo
        return -1;
    }
    Cabecalho cab;
    int RRN_anterior;
    le_cabecalho(arq, &cab);
    RRN_anterior = cab.topo;    // O topo da pilha é repassado para o último elemento removido
    cab.topo = RRN_removido;
    grava_cabecalho(arq, cab);  // Gravação da mudança no cabeçalho
    return RRN_anterior;
}

// Quando há alguma inserção bem-sucedida, o próximo RRN disponível será aumentado
void aumenta_RRN(FILE *arq, int qtd_insercao){
    if(arq == NULL){            
        msg_arquivo_inexistente();  // Não foi possível manipular o arquivo
        return;
    }
    if(qtd_insercao <=0){   // Caso tenha algum erro na chamada da função
        msg_erro_padrao(); 
        return;
    }
    Cabecalho cab;
    le_cabecalho(arq, &cab);
    cab.proxRRN += qtd_insercao; // Atualiza o próximo RRN disponível para uma inserção no final do arquivo
    grava_cabecalho(arq, cab);
}

// Quando há alguma compactação bem-sucedida, o próximo RRN disponível será diminuído
void diminui_RRN(FILE *arq, int qtd_compactacao){
    if(arq == NULL){            
        msg_arquivo_inexistente();  // Não foi possível manipular o arquivo
        return;
    }
    if(qtd_compactacao <=0){
        msg_erro_padrao();
        return;
    }
    Cabecalho cab;
    fseek(arq, 0, SEEK_SET);
    le_cabecalho(arq, &cab);
    cab.proxRRN -= qtd_compactacao; // Atualiza o próximo RRN disponível para uma inserção no final do arquivo

    // Não é possível retirar mais registros do que os existentes
    if(cab.proxRRN < 0){
        msg_erro_padrao();
        return;
    }
    grava_cabecalho(arq, cab);
}

// Quando há alguma remoção bem-sucedida, aumenta o número de remoções no cabeçalho
void atualiza_remocao(FILE *arq, int qtd_remocao){
    if(arq == NULL){            
        msg_arquivo_inexistente();  // Não foi possível manipular o arquivo
        return;
    }
    Cabecalho cab;
    le_cabecalho(arq, &cab);
    cab.nroRegRem += qtd_remocao; // Atualiza o número de registros o arquivo
    grava_cabecalho(arq, cab);
}

// Quando há aumento (inserção) ou diminuição (compactação) da quantidade de bytes do arquivo,
// pode ser alterada a quantidade de páginas de disco. Então, modifica-se essa variável no cabeçalho
void atualiza_PagDisco(FILE *arq){
    if(arq == NULL){            
        msg_arquivo_inexistente();  // Não foi possível manipular o arquivo
        return;
    }
    int qtd_paginas;
    Cabecalho cab;
    // Verifica quantas páginas o arquivo tem
    fseek(arq, 0, SEEK_END);
    qtd_paginas = (ftell(arq))/1600;   
    if (ftell(arq) % 1600 > 0) {
        qtd_paginas += 1; // Adiciona uma página se houver resto na divisão
    }
    // Verifica quantas páginas estão marcadas no cabeçalho
    le_cabecalho(arq, &cab);
    if(qtd_paginas != cab.nroPagDisco){     // Se forem diferentes, quer dizer que houve mudança significativa 
        cab.nroPagDisco = qtd_paginas;
        grava_cabecalho(arq, cab);
    }
}     

// Quando há uma compactação bem-sucedida, incrementa a quantidade de compactações no cabeçalho
void atualiza_qttCompacta(FILE *arq, int qtd_compactacao){
    if(arq == NULL){            
        msg_arquivo_inexistente();  // Não foi possível manipular o arquivo
        return;
    }
    if(qtd_compactacao <=0){    // Não é possível diminuir a quantidade de compactações
        msg_erro_padrao();
        return;
    }
    Cabecalho cab;
    le_cabecalho(arq, &cab);
    cab.qttCompacta += qtd_compactacao;
    grava_cabecalho(arq, cab);
}

