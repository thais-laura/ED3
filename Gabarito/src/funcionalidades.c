#include "./funcionalidades.h"

#include "funcionalidades.h"

void funcionalidade1() {
    int numInsercoes;
    char arquivo_nome[100];
    FILE *arquivo = NULL;

    scanf(" %s %d", arquivo_nome, &numInsercoes);

    // Abrindo um novo arquivo binário no modo de escrita
    arquivo = fopen(arquivo_nome, "wb");
    if (arquivo == NULL) {
        msg_arquivo_inexistente();
        return;
    }

    for (int i = 0; i < numInsercoes; i++) {
        // Leitura (stdin) de uma espécie a ser inserida no arquivo
        ESPECIE *especie = especie_ler();
        if (especie == NULL) {
            fclose(arquivo);
            return;
        }
        // Escrita da espécie no arquivo binário
        if(!especie_escrever_arquivo(especie, arquivo)){
            fclose(arquivo); // Houve algum erro de escrita...
            msg_erro_padrao();
            return;
        }
        especie_apagar(&especie);
    }

    fclose(arquivo);

    binarioNaTela(arquivo_nome);
}


void funcionalidade2() {
    char arquivo_nome[100];
    FILE *arquivo = NULL;

    scanf(" %s", arquivo_nome);
    
    // Abrindo um arquivo binário existente no modo de leitura
    arquivo = fopen(arquivo_nome, "rb");
    if (arquivo == NULL) {
        msg_arquivo_inexistente();
        return;
    }

    while (true) { // Enquanto tiver espécies a serem lidas...
        ESPECIE *especie = especie_ler_arquivo(arquivo);
        if (especie == NULL) {
            break;
        }
        especie_imprimir(especie);
        especie_apagar(&especie);
    }

    fclose(arquivo);
}


void funcionalidade3() {
    int rrn; // Pois estamos lidando com registros de tamanho fixo
    char arquivo_nome[100];
    FILE *arquivo = NULL;

    scanf(" %s %d", arquivo_nome, &rrn);

    // Abrindo um arquivo binário existente no modo de leitura
    arquivo = fopen(arquivo_nome, "rb");
    if (arquivo == NULL) {
        msg_arquivo_inexistente();
        return;
    }

    fseek(arquivo, 0, SEEK_END); // Indo para o final do arquivo
    int ultimo_rrn =  ftell(arquivo) / TAM_ESPECIE - 1;

    // Verificando se o RRN passado representa um 
    // registro que está dentro dos limites do arquivo
    if (rrn < 0 || rrn > ultimo_rrn) {
        msg_rrn_invalido();
    }

    // Cálculo do byteOffset do registro desejado
    int byteoffset = rrn * TAM_ESPECIE;
    fseek(arquivo, byteoffset, SEEK_SET);
    ESPECIE *especie = especie_ler_arquivo(arquivo);
    if (especie == NULL) {
        fclose(arquivo);
        return;
    }

    especie_imprimir(especie);
    especie_apagar(&especie);

    fclose(arquivo);
}


void funcionalidade4() {
    char arquivo_nome[100];
    FILE *arquivo = NULL;

    scanf(" %s", arquivo_nome);

    // Abrindo um arquivo binário existente no modo de leitura e escrita
    arquivo = fopen(arquivo_nome, "rb+");
    if (arquivo == NULL) {
        msg_arquivo_inexistente();
        return;
    }

    int idEspecie = 0;
    scanf(" %d", &idEspecie);

    int numEdicoes = 0;
    scanf(" %d", &numEdicoes);

    bool encontrado = false; // Identidica se o registro desejado foi encontrado no arquivo de dados
    long long int byteOffset = 0; // Indica o byteOffset do registro desejado no arquivo de dados

    // Realizando busca sequencial no arquivo de dados
    // Note que a busca é interrompida quando idEspecie é encontrado
    ESPECIE *especie = buscar_especie(idEspecie, arquivo, &encontrado, &byteOffset);
    if(!encontrado){
        msg_species_id_inexistente();
    } else {
        for(int i=0; i<numEdicoes; i++){
            // id_species válido, realizar leitura (stdin) dos campos a serem alterados
            CRITERIOS_EDICAO *criterio_edicao = ler_criterios_edicao();
            if(criterio_edicao == NULL){ // Houve algum erro de alocação
                especie_apagar(&especie); // ou entrada fora do padrão
                msg_erro_padrao();
                fclose(arquivo);
                return;
            }
            // Verificar se o dado já foi cadastrado para emitir mensagem de erro
            bool jaCadastrado = verificar_dado_ja_cadastrado(especie, criterio_edicao);
            if(jaCadastrado){
                msg_dados_ja_cadastrados();
            } else {
                // Realizando a alteração do campo no arquivo de dados
                editar_campo(criterio_edicao, arquivo, byteOffset);
            }
            free(criterio_edicao);
        }
    }

    fclose(arquivo);
    especie_apagar(&especie);

    binarioNaTela(arquivo_nome);
}