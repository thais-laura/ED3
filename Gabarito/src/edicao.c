#include "./edicao.h"

struct _criterios_edicao {              // Estrutura auxiliar da funcionalidade 4
    bool isPopulation;                  // Para cada edição, registra qual é o campo a
    int population;                     // ser alterado e qual é o valor do novo campo
    bool isStatus;
    char status[TAM_STATUS];
    bool isHumanImpact;
    int human_impact;
};

CRITERIOS_EDICAO *ler_criterios_edicao(){

    // Alocando uma estrutura auxiliar para armazenar os critérios de edição
    CRITERIOS_EDICAO *criterio_edicao = (CRITERIOS_EDICAO *)malloc(sizeof(CRITERIOS_EDICAO));
    if (criterio_edicao == NULL) {
        return NULL;
    }
    criterio_edicao->isHumanImpact = false;
    criterio_edicao->isPopulation = false;
    criterio_edicao->isStatus = false;

    char campo[100];
    readline(campo); // Pois o nome dos campos serão sempre divididos por '\n'

    // Para cada uma das possibilidades de campo editáveis,
    // ler o novo valor e atribuir à struct auxiliar

    if(strcmp(campo, "STATUS") == 0){
        criterio_edicao->isStatus = true;
        readline(criterio_edicao->status);
        criterio_edicao->status[strlen(criterio_edicao->status)+1] = '\0';
        return criterio_edicao;
    }

    else if(strcmp(campo, "HUMAN IMPACT") == 0){
        criterio_edicao->isHumanImpact = true;
        scanf("%d", &criterio_edicao->human_impact);
        getchar();
        return criterio_edicao;
    }

    else if(strcmp(campo, "POPULATION") == 0){
        criterio_edicao->isPopulation = true;
        scanf("%d", &criterio_edicao->population);
        getchar();
        return criterio_edicao;
    }

    else {
        msg_erro_padrao(); // O usuário digitou um nome de campo desconhecido
    }

    return NULL;

}

bool verificar_dado_ja_cadastrado(ESPECIE *especie, CRITERIOS_EDICAO *criterio_edicao){
    // Para cada uma das possibilidades de campo editáveis, verificar
    // se o valor do campo marcado no arquivo de dados não está nulo
    if(criterio_edicao->isPopulation == true){
        if(especie->population != 0){
            return true; // Dado não está marcado como nulo
        }
    }
    if(criterio_edicao->isStatus == true){
        if(strcmp(especie->status, "NULO") != 0){
            return true;
        }
    }
    if(criterio_edicao->isHumanImpact == true){
        if(especie->human_impact != 0){
            return true;
        }
    }
    return false; // Dado está marcado como nulo
}

void editar_campo(CRITERIOS_EDICAO *criterio_edicao, FILE *arquivo, long long int byteOffset){
    if(criterio_edicao->isPopulation == true){
        // Pulando para a posição de início do campo population
        fseek(arquivo, byteOffset + 4 + TAM_NOME + TAM_NOME_CIENTIFICO, SEEK_SET);
        fwrite(&(criterio_edicao->population), sizeof(int), 1, arquivo);
    }
    else if(criterio_edicao->isStatus == true){
        // Pulando para a posição de início do campo status
        fseek(arquivo, byteOffset + 4 + TAM_NOME + TAM_NOME_CIENTIFICO + 4, SEEK_SET);
        _arquivo_escrever_string(arquivo, criterio_edicao->status, TAM_STATUS);
    }
    else if(criterio_edicao->isHumanImpact == true){
        // Pulando para a posição de início do campo human_impact
        fseek(arquivo, byteOffset + 4 + TAM_NOME + TAM_NOME_CIENTIFICO + 4 + TAM_STATUS + 8, SEEK_SET);
        fwrite(&criterio_edicao->human_impact, sizeof(int), 1, arquivo);
    }
}