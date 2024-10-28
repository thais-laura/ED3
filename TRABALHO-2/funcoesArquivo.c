#include "./cabecalho.h"
#include "./funcionalidades.h"
#include "./funcoesArquivo.h"
#include "./funcoesArvore.h"
#include "./funcoesFornecidas.h"
#include "./manipularVariaveis.h"
#include "./mensagens.h"
#include "./structs.h"

// Função para escrever os registros no arquivo binário, tratando valores nulos
void escreve_registro(FILE *arq, Especie *esp) {
    if(arq == NULL){            
        msg_arquivo_inexistente();  // Não foi possível manipular o arquivo
        return;
    }

    int antes = ftell(arq);
    // Gravar campos de controle
    fwrite(&esp->removido, sizeof(char), 1, arq);       
    fwrite(&esp->encadeamento, sizeof(int), 1, arq);     

    // Grava o restante dos campos fixos
    fwrite(&esp->populacao, sizeof(int), 1, arq);  
    fwrite(&esp->tamanho, sizeof(float), 1, arq); 
    fwrite(&esp->unidadeMedida, sizeof(char), 1, arq); 
    fwrite(&esp->velocidade, sizeof(int), 1, arq);

    // Gravar campos de string (tamanho variável) no arquivo, manipulando nulos
    grava_str(arq, esp->nome);
    grava_str(arq, esp->especie);
    grava_str(arq, esp->habitat);
    grava_str(arq, esp->tipo);
    grava_str(arq, esp->dieta);
    grava_str(arq, esp->alimento);

    // Preenche com '$' o restante do registro (tamanho fixo - 160 bytes)
    int depois = ftell(arq);
    char lixo = '$';
    for (int i = 0; i < 160 - (depois-antes); i++) {    // A diferença de 'depois' e 'antes' indica o tamanho ocupado pelos campos
        fwrite(&lixo, sizeof(char), 1, arq);  
    }
}

/* Essa função lê todos os campos fixos do registro e
retorna a quantidade de bytes lidos */
int le_camposFixos(FILE *arq, Especie *esp) {
    if (arq == NULL) {
        msg_arquivo_inexistente();  // Não foi possível manipular o arquivo
        return -1;  // Retorna erro
    }
    int antes = ftell(arq);  
    // O retorno -1 indica que não foi possível realizar a leitura dos campos
    if (fread(&(esp->removido), sizeof(char), 1, arq) != 1) return -1;
    if (fread(&(esp->encadeamento), sizeof(int), 1, arq) != 1) return -1;
    if (fread(&(esp->populacao), sizeof(int), 1, arq) != 1) return -1;
    if (fread(&(esp->tamanho), sizeof(int), 1, arq) != 1) return -1;
    if (fread(&(esp->unidadeMedida), sizeof(char), 1, arq) != 1) return -1;
    if (fread(&(esp->velocidade), sizeof(int), 1, arq) != 1) return -1;

    int depois = ftell(arq);  
    return (depois - antes);  // Retorna o número de bytes lidos
}
/* Essa função lê todos os campos de tamanho variável do registro e 
retorna a quantidade de bytes lidos. É interessante observar que as 
strings são delimitadas pelo caractere '#' e não pelo '\0' */
int le_camposVariaveis(FILE *arq, Especie *esp) {
    if(arq == NULL){
        msg_arquivo_inexistente();  // Não foi possível manipular o arquivo
        return -1;  // Retorna erro
    }
    int antes = ftell(arq);
    le_str(arq, &(esp->nome));      
    le_str(arq, &(esp->especie));   
    le_str(arq, &(esp->habitat));
    le_str(arq, &(esp->tipo));
    le_str(arq, &(esp->dieta));
    le_str(arq, &(esp->alimento));
    
    int depois = ftell(arq);
    return (depois - antes);  // Retorna o número de bytes lidos
}

// Essa função imprime todos os campos do registro que não sejam nulos
void imprime_registro(Especie esp){
    if(! verifica_stringNula(esp.nome)) printf("Nome: %s\n", esp.nome);
    if(! verifica_stringNula(esp.especie)) printf("Especie: %s\n", esp.especie);
    if(! verifica_stringNula(esp.tipo)) printf("Tipo: %s\n", esp.tipo);
    if(! verifica_stringNula(esp.dieta)) printf("Dieta: %s\n", esp.dieta);
    if(! verifica_stringNula(esp.habitat)) printf("Lugar que habitava: %s\n", esp.habitat);
    if(esp.tamanho != -1.0) printf("Tamanho: %.1f m\n", esp.tamanho);
    if(esp.velocidade != -1) printf("Velocidade: %d %cm/h\n", esp.velocidade, esp.unidadeMedida);
    printf("\n");
}

