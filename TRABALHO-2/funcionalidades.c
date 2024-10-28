#include "./cabecalho.h"
#include "./cabecalhoArv.h"
#include "./funcionalidades.h"
#include "./funcoesArquivo.h"
#include "./funcoesArvore.h"
#include "./funcoesFornecidas.h"
#include "./manipularVariaveis.h"
#include "./mensagens.h"
#include "./structs.h"


/*Implementa a funcionalidade de criar um arquivo de �ndice (�rvore B) a partir de um arquivo de dados.*/
void funcionalidade7(char *nome_ardDados, char *nome_arqInd) {
    FILE *arqDados = fopen(nome_ardDados, "rb+");
    if (arqDados == NULL) {
        msg_arquivo_inexistente();
        return;
    }
    if(atualiza_status(arqDados, '0')){
        msg_erro_padrao();
        fclose(arqDados);
        return;
    }

    FILE *arqInd = fopen(nome_arqInd, "wb+");
    if (arqInd == NULL) {
        fclose(arqDados);
        msg_arquivo_inexistente();
        return;
    }
    // Verifica o status do arquivo de dados para assegurar que est� consistente
    Cabecalho cabecalhoDados;
    le_cabecalho(arqDados, &cabecalhoDados);
    if (cabecalhoDados.proxRRN == 0) {
        atualiza_status(arqDados, '1');
        fclose(arqDados);
        fclose(arqInd);
        msg_registro_inexistente();
        return;
    }
    inicia_cabecalhoArv(arqInd);
    CabecalhoArv cabecalhoArv;
    le_cabecalhoArv(arqInd, &cabecalhoArv);
    if(atualiza_statusArv(arqInd, '0')){
        msg_erro_padrao();
        atualiza_status(arqDados, '1');
        fclose(arqDados);
        fclose(arqInd);
        return;
    }

    cria_indice(arqDados, arqInd, cabecalhoDados, cabecalhoArv);


    atualiza_status(arqDados, '1');
    atualiza_statusArv(arqInd, '1');  // Atualiza o status para consistente

    // Fecha os arquivos
    fclose(arqDados);
    fclose(arqInd);

    // Exibe o arquivo de �ndice na tela
    binarioNaTela(nome_arqInd);
}



/* Recuperação dos dados de acordo com a busca do campo "nome".
Primeiramente, ocorre a busca no arquivo de índices, encontra-se o campo de referência
e logo depois recupera o registro no arquivo de dados */
void funcionalidade8(char *nome_arqDados, char *nome_arqInd, char *nome){
    FILE *arqInd = fopen(nome_arqInd, "rb+");
    if(arqInd == NULL){                         // Não foi possível manipular o arquivo
        msg_arquivo_inexistente();
        return;
    }if(atualiza_statusArv(arqInd, '0')){       // Atualiza o arquivo como inconsistente caso ele esteja consistente
        fclose(arqInd);
        return;
    }
    CabecalhoArv cabArv;
    le_cabecalhoArv(arqInd, &cabArv);
    if(cabArv.noRaiz == -1){                    // Árvore vazia
        msg_registro_inexistente();
        atualiza_statusArv(arqInd, '1');        // Atualiza como consistente novamente
        fclose(arqInd);
        return;
    }

    // A chave inserida no arquivo de índices é a conversão da string nome para um long int
    long int chave = converteNome(nome), byteoffset;    
    int RRN_pagDisco, RRN_pai;
    // Busca da chave e retorno da posição no arquivo de dados
    int encontrado = buscaArv(arqInd, chave, &byteoffset, &RRN_pagDisco, &RRN_pai);

    atualiza_statusArv(arqInd, '1');        // Atualiza como consistente novamente
    fclose(arqInd);     
    if(encontrado == -1){                   // Chave não encontrada
        msg_registro_inexistente();
        return;
    }

    // Início da manipulação do arquivo de dados para busca do registro correspondente
    FILE *arqDados = fopen(nome_arqDados, "rb+");
    if(arqDados == NULL){                           // Não foi possível manipular o arquivo
        msg_arquivo_inexistente();
        return;
    }if(atualiza_status(arqDados, '0')){            // Atualiza o arquivo como inconsistente caso ele esteja consistente
        fclose(arqDados);
        return;
    }
    Cabecalho cab;
    le_cabecalho(arqDados, &cab);
    if((byteoffset-1600)/160 >= cab.proxRRN){       // Verifica se existe o registro no arquivo de dados por meio do RRN
        msg_registro_inexistente();
        atualiza_status(arqDados, '1');             // Atualiza como consistente novamente
        fclose(arqDados);
        return;
    }
    fseek(arqDados, byteoffset, SEEK_SET);          // Move o cursor para a posição do registro desejado
    Especie esp;
    le_camposFixos(arqDados, &esp);
    if(esp.removido == '1'){                        // Verifica se o registro está removido logicamente
        msg_registro_inexistente();
        atualiza_status(arqDados, '1');             // Atualiza como consistente novamente
        fclose(arqDados);
        return;
    }
    le_camposVariaveis(arqDados, &esp);
    imprime_registro(esp);                          // Impressão do registro encontrado

    atualiza_status(arqDados, '1');                 // Atualiza como consistente novamente
    fclose(arqDados);
}

/* A funcionalidade 9 é uma extensão da funcionalidade 5: ao inserir novos registros
no lugar de registros removidos no arquivo de dados, a chave correspondente ao nome 
da espécie é inserida na árvore B para atualizar o arquivo de índices. */
void funcionalidade9(char *nome_arqDados,  char *nome_ardInd, int qtd){
    FILE *arqDados = fopen(nome_arqDados, "rb+");
    if(arqDados == NULL){                            	// Não foi possível manipular o arquivo
        msg_arquivo_inexistente();
        return;
    }
    if(atualiza_status(arqDados, '0'))               	// Define o status do arquivo como inconsistente se antes estiver consistente
        return;

    FILE *arqInd = fopen(nome_ardInd, "rb+");
    if(arqInd == NULL){                            	    // Não foi possível manipular o arquivo
        msg_arquivo_inexistente();
        return;
    }
    if(atualiza_statusArv(arqInd, '0'))               	// Define o status do arquivo como inconsistente se antes estiver consistente
        return;

    Cabecalho cabDados;
    le_cabecalho(arqDados, &cabDados);
    CabecalhoArv cabArv;
    le_cabecalhoArv(arqInd, &cabArv);
    Especie esp;
    long int chave;
    for(int i=0; i<qtd; i++){
        // Leitura dos campos do novo registro
        le_e_aloca_string(&esp.nome);
        le_e_aloca_string(&esp.dieta);
        le_e_aloca_string(&esp.habitat);

        esp.populacao = le_inteiro_ou_nulo();
        le_e_aloca_string(&esp.tipo);
        esp.velocidade = le_inteiro_ou_nulo();
        esp.unidadeMedida = le_char_ou_nulo();
        esp.tamanho = le_inteiro_ou_nulo();

        le_e_aloca_string(&esp.especie);
        le_e_aloca_string(&esp.alimento);

        le_cabecalho(arqDados, &cabDados);                      // A variável do cabeçalho muda conforme as inserções nos removidos são feitas
        if(cabDados.nroRegRem == 0 && cabDados.topo == -1){     // Caso não tenha registros removidos, insere no final do arquivo
            fseek(arqDados, 0, SEEK_END);
            int byteoffset = ftell(arqDados);
            esp.removido = '0';                                 // Registro novo ainda não foi removido
            esp.encadeamento = -1;                              // Não faz parte da pilha lógica
            escreve_registro(arqDados, &esp);

            // Atualização do arquivo de índices 
            chave = converteNome(esp.nome);
            insereNo(arqInd, chave, byteoffset, &cabArv);

            aumenta_RRN(arqDados, 1);                            // A cada inserção no final do arquivo há um aumento no próximo RRN disponível
            atualiza_PagDisco(arqDados);                         // A cada inserção, pode-se mudar a quantidade de páginas de disco
        }else{
            Especie aux;
            int ultimoRRNrem = cabDados.topo;                    // Indica o último registro removido, onde será inserido o novo registro
            fseek(arqDados, 1600 + (ultimoRRNrem * 160), SEEK_SET);
            int tam_fixo = le_camposFixos(arqDados, &aux);
            int proxRRNrem = aux.encadeamento;                  // O penúltimo registro removido será novamente encadeado no topo da pilha lógica
            esp.removido = '0';
            esp.encadeamento = -1;
            fseek(arqDados, -tam_fixo, SEEK_CUR);                // É necessário voltar para o início do registro
            escreve_registro(arqDados, &esp);
            atualiza_topo(arqDados, proxRRNrem);                 // Muda-se o topo da pilha
            atualiza_remocao(arqDados, -1);                      // Um registro removido foi substituído por outro

            // Atualização do arquivo de índices 
            chave = converteNome(esp.nome);
            insereNo(arqInd, chave, 1600 + (ultimoRRNrem * 160), &cabArv);
        }

        // Liberação da memória alocada dinamicamente
        free(esp.nome);
        free(esp.dieta);
        free(esp.habitat);
        free(esp.alimento);
        free(esp.tipo);
        free(esp.especie);
    }
    // Indicativo que se finalizou a manipulação do arquivo
    atualiza_status(arqDados, '1');     
    atualiza_statusArv(arqInd, '1');
    fclose(arqDados);
    fclose(arqInd);

    binarioNaTela(nome_ardInd);
}