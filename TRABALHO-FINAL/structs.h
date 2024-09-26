#ifndef STRUCTS_H_INCLUDED
#define STRUCTS_H_INCLUDED

// Diferentes structs para lidar com registros de dados, cabeçalho e campos dos registros

typedef struct registro_dados {
    // Campos fixos
    int populacao;          // Pode ser nulo e repetido
    float tamanho;          // Pode ser nulo e repetido
    char unidadeMedida;     // Pode ser nulo e repetido
    int velocidade;         // Pode ser nulo e repetido
    // Campos variáveis
    char *nome;             // Não pode ser nulo nem repetido
    char *especie;          // Não pode ser nulo nem repetido
    char *habitat;          // Pode ser nulo e repetido
    char *tipo;             // Pode ser nulo e repetido
    char *dieta;            // Não pode ser nulo mas pode ser repetido
    char *alimento;         // Pode ser nulo
    // Campos de controle
    char removido;          // '1' ou '0'
    int encadeamento;       // Caso não esteja na pilha, é dado como -1
} Especie;

typedef struct registro_cabecalho{
    char status;            // Arquivo consistente ('1') ou não ('0')
    int topo;               // Topo da pilha lógica de removidos
    int proxRRN;            // Próximo RRN disponível
    int nroRegRem;          // Quantidade de registros removidos 
    int nroPagDisco;        // Quantidade de páginas de disco
    int qttCompacta;        // Quantidade de compactações feitas
} Cabecalho;

// Definição de tipos de campo para identificar o tipo de dado armazenado
typedef enum {
    TIPO_INT,
    TIPO_FLOAT,
    TIPO_CHAR,
    TIPO_STRING
} TipoDado;

// União para armazenar diferentes tipos de dados em um único lugar
typedef union {
    int i;
    float f;
    char c;
    char str[50];
} Valor;

typedef struct {
    char campo[20];   // Nome do campo (população, espécie, ...)
    TipoDado tipo;    // Tipo do dado (int, float, char ou string)
    Valor valor;      // Valor do campo (relaciona-se ao tipo do dado)
} CampoValor;

#endif //STRUCTS_H_INCLUDED