#include "./manipularVariaveis.h"
#include "./funcoesFornecidas.h"
#include "./mensagens.h"

// Devido à imprecisão do ponto flutuante, é preciso trabalhar com um intervalo de semelhança
int verifica_float(float a, float b){   
    if(a-b < 0.001 && a-b > -0.001)     // É permitida uma variação de até 0.001 em módulo
        return 1;
    else return 0;
}

void le_e_aloca_string(char **campo) {
    char aux[50];
    scan_quote_string(aux);
    
    *campo = malloc((strlen(aux) + 1) * sizeof(char)); // Aloca o espaço necessário
    if (*campo == NULL) {
        fprintf(stderr, "Erro de alocação de memória.\n");
        exit(1); // Ou outra forma de tratar o erro
    }
    
    strcpy(*campo, aux); // Copia o conteúdo
}

int le_inteiro_ou_nulo() {
    char aux[50];
    scan_quote_string(aux);
    
    if (strcmp(aux, "") == 0) {
        return -1; // Retorna valor de "nulo"
    } else {
        return atoi(aux); // Converte para inteiro
    }
}

char le_char_ou_nulo() {
    char aux[50];
    scan_quote_string(aux);
    
    if (strcmp(aux, "") == 0) {
        return '$'; // Valor de "nulo" para o caractere
    } else {
        return aux[0]; // Retorna o primeiro caractere
    }
}

// Essa função verifica se a string é nula e pode ser impressa
int verifica_stringNula(char *campo){
    if(campo == NULL) return 1;

    if(campo[0] == '$'|| !strcmp(campo, "") || campo[0] == '\0')
        return 1;       // Campo nulo
    else return 0;      // Campo não nulo
}

/* Essa função permite gravar uma string no arquivo binário
sem o finalizador (\0) e, ainda, adiciona-se o delimitador de campo */
void grava_str(FILE *arq, char *str) {
    if (arq == NULL) {
        msg_arquivo_inexistente();
        return;
    }
    if (str == NULL || strlen(str) == 0) {  // Se a string for nula ou vazia
        char c = '#';                       // Grava apenas um '#' para indicar nulo
        fwrite(&c, sizeof(char), 1, arq);
    } else {
        int tam = strlen(str);
        char c = '#';
        if(str[tam-1]=='\n'){
            str[tam-1]='\0';
            tam--;
        }
        fwrite(str, sizeof(char), tam, arq);
        fwrite(&c, sizeof(char), 1, arq);  // Grava o '#' no final da string
    }
}

/* Essa função lê uma string no arquivo binário
Como o campo é variável, deve-se ler de char em char para que se saiba onde termina.
A condição de parada é o delimitador do campo: #
Por fim, adiciona-se o '\0', o qual indica o fim da string */
void le_str(FILE *arq, char **str) {
    if(arq == NULL){            
        msg_arquivo_inexistente();  // Não foi possível manipular o arquivo
        return;
    }

    int tam = 50;       // Tamanho inicial da string
    int i = 0;          // Indica a posição da string
    char c;             // Buffer para caractere

    // Aloca memória inicial
    *str = (char *)malloc(tam * sizeof(char));
    if (*str == NULL) {
        msg_alocacao_mem();
        return;
    }

    // Lê de char em char até encontrar o delimitador '#'
    while (fread(&c, sizeof(char), 1, arq) == 1 && c != '#') {
        // Se o índice ultrapassar o tamanho alocado, realoca mais memória
        if (i >= tam - 1) {  // -1 para garantir espaço para o '\0'
            tam *= 2;        // Dobra o tamanho da alocação
            char *temp = realloc(*str, tam * sizeof(char));
            if (temp == NULL) {
                msg_alocacao_mem();
                free(*str);   // Libera a memória alocada anteriormente
                return;
            }
            *str = temp;      // Atualiza o ponteiro após realocação
        }
        (*str)[i++] = c;
    }
    (*str)[i] = '\0';         // Marca o fim da string que foi retirada antes de inseri-la no arquivo
}

