#include "./mensagens.h"

void msg_reg_removido(){
    printf("Registro inexistente.\n");
}
void msg_rrn_invalido() {
    printf("RRN invalido\t Falha no processamento do arquivo.\n");
}

void msg_registro_inexistente() {
    printf("Registro inexistente.\n");
}

void msg_arquivo_inexistente() {
    printf("Falha no processamento do arquivo.\n");
}

void msg_dados_ja_cadastrados() {
    printf("Informação já inserida no arquivo.\n");
}

void msg_funcionalidade_inexistente() {
    printf("A funcionalidade não foi encontrada.\n");
}

void msg_erro_padrao(){
    printf("Falha no processamento do arquivo.\n");
}

void msg_alocacao_mem(){
    printf("Falha na alocação de memória.\n");
}
