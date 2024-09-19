#include "./especies.h"

void especie_apagar(ESPECIE **especie) {
    if (especie == NULL) {
        return;
    }
    free(*especie);
    *especie = NULL;
}

ESPECIE* especie_ler() {
    // Alocando uma struct espécie
    ESPECIE *especie = (ESPECIE *)malloc(sizeof(ESPECIE));
    if (especie == NULL) {
        return NULL;
    }

    // Lendo da entrada padrão cada campo da struct espécie
    scanf("%d", &especie->species_id);
    getchar(); // Consumir o '\n' linha após o inteiro

    readline(especie->name); // Lendo o nome com espaços (função fornecida)
    especie->name[strlen(especie->name)+1] = '\0'; // É importante garantir a presença do '\0'
                                                  // pois deve ser gravado no arquivo de dados
    readline(especie->scientific_name);
    especie->scientific_name[strlen(especie->scientific_name)+1] = '\0';

    scanf("%d", &especie->population);
    getchar();

    readline(especie->status);
    especie->status[strlen(especie->status)+1] = '\0';

    scanf("%f %f", &especie->location[0], &especie->location[1]);
    getchar();

    scanf("%d", &especie->human_impact);
    getchar();

    return especie;
}

void especie_imprimir(ESPECIE *especie) {
    if (especie == NULL) {
        return;
    }

    printf("ID: %d\n", especie->species_id);
    printf("Nome: %s\n", especie->name);
    printf("Nome Científico: %s\n", especie->scientific_name);

    if(especie->population != 0){
        printf("População: %d\n", especie->population);
    } else {
        printf("População: NULO\n");
    }

    printf("Status: %s\n", especie->status);

    printf("Localização: (%.2f, %.2f)\n", especie->location[0], especie->location[1]);

    if(especie->human_impact != 0){
        printf("Impacto Humano: %d\n\n", especie->human_impact);
    } else {
        printf("Impacto Humano: NULO\n\n");
    }
}

ESPECIE *especie_ler_arquivo(FILE *arquivo) {
    if (arquivo == NULL) {
        return NULL;
    }
    ESPECIE *especie = (ESPECIE *)malloc(sizeof(ESPECIE));
    if (especie == NULL) {
        return NULL;
    }
    // Lendo species_id. É uma boa prática verificar se a função fread 
    // retornou corretamente a quantidade de itens que foram lidos
    if (fread(&especie->species_id, sizeof(int), 1, arquivo) != 1) {
        especie_apagar(&especie); // Leitura falhou...
        return NULL;
    }
    // Lendo name
    if (fread(especie->name, sizeof(char), TAM_NOME, arquivo) != TAM_NOME) {
        especie_apagar(&especie);
        return NULL;
    }
    // Lendo scientific_name
    if (fread(especie->scientific_name, sizeof(char), TAM_NOME_CIENTIFICO, arquivo) != TAM_NOME_CIENTIFICO) {
        especie_apagar(&especie);
        return NULL;
    }
    // Lendo population
    if (fread(&especie->population, sizeof(int), 1, arquivo) != 1) {
        especie_apagar(&especie);
        return NULL;
    }
    // Lendo status
    if (fread(especie->status, sizeof(char), TAM_STATUS, arquivo) != TAM_STATUS) {
        especie_apagar(&especie);
        return NULL;
    }
    // Lendo location (X e Y)
    if (fread(especie->location, sizeof(float), 2, arquivo) != 2) {
        especie_apagar(&especie);
        return NULL;
    }
    // Lendo human_impact
    if (fread(&especie->human_impact, sizeof(int), 1, arquivo) != 1) {
        especie_apagar(&especie);
        return NULL;
    }
    return especie;
}

bool _arquivo_escrever_string(FILE *arquivo, char *str, int tam) {
    if (arquivo == NULL || str == NULL) {
        return false;
    }

    // Escreve a string seguida do terminador nulo
    int len = strlen(str) + 1; // +1 por conta do '\0'
    if(fwrite(str, sizeof(char), len, arquivo) != len){
        return false; // Escrita falhou...
    }

    // Preenche o restante com o caractere '$'
    char lixo = '$';
    for(int i=len; i<tam; i++){ // Escrita char a char
        if (fwrite(&lixo, sizeof(char), 1, arquivo) != 1) {
            return false; // Escrita falhou...
        }
    }
    return true;
}

bool especie_escrever_arquivo(ESPECIE *especie, FILE *arquivo) {
    if (arquivo == NULL || especie == NULL) {
        return false;
    }
    // Escrevendo species_id
    if (fwrite(&especie->species_id, sizeof(int), 1, arquivo) != 1) {
        return false; // Escrita falhou...
    }
    // Escrevendo name
    if (!_arquivo_escrever_string(arquivo, especie->name, TAM_NOME)) {
        return false;
    }
    // Escrevendo scientific_name
    if (!_arquivo_escrever_string(arquivo, especie->scientific_name, TAM_NOME_CIENTIFICO)) {
        return false;
    }
    // Escrevendo population
    if (fwrite(&especie->population, sizeof(int), 1, arquivo) != 1) {
        return false;
    }
    // Escrevendo status
    if (!_arquivo_escrever_string(arquivo, especie->status, TAM_STATUS)) {
        return false;
    }
    // Escrevendo location (X e Y)
    if (fwrite(&especie->location[0], sizeof(float), 1, arquivo) != 1) {
        return false;
    }
    if (fwrite(&especie->location[1], sizeof(float), 1, arquivo) != 1) {
        return false;
    }
    // Escrevendo human_impact
    if (fwrite(&especie->human_impact, sizeof(int), 1, arquivo) != 1) {
        return false;
    }
    return true;
}

ESPECIE *buscar_especie(int idEspecie, FILE *arquivo, bool *encontrado, long long int *byteOffset){
    // Enquanto tiver espécies a serem lidas
    while (true) {
        // Obtendo a posição de início do atual registro
        *byteOffset = ftell(arquivo);
        ESPECIE *especie = especie_ler_arquivo(arquivo);
        if (especie == NULL) {
            break; // Chegamso ao final do arquivo ou algum erro ocorreu
        }
        if(especie->species_id == idEspecie){
            // id_species válido e encontrado
            *encontrado = true;
            return especie;
        }
        especie_apagar(&especie);
    }
    *encontrado = false;
    return NULL;
}