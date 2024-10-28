#include "./cabecalho.h"
#include "./funcionalidades.h"
#include "./funcoesArquivo.h"
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

// Função que lê um dos campos do registro diretamente de um arquivo CSV
void le_campoCSV(char **destino, char **linha) {
    // Encontra a próxima vírgula (divisor de campos) ou o fim da linha
    char *virgula = strchr(*linha, ',');
    if (virgula == NULL)                    // Último campo da linha
        virgula = *linha + strlen(*linha);  // Marca o final da string
    
    size_t tam = virgula - *linha;          // Tamanho do campo atual

    if (tam == 0) {
        // Campo vazio
        *destino = (char *)malloc(2 * sizeof(char));
        strcpy(*destino, "\0");
    } else {
        // Campo não vazio, copia o conteúdo
        *destino = (char *)malloc((tam + 1) * sizeof(char));
        strncpy(*destino, *linha, tam);
        (*destino)[tam] = '\0';  // Certifica que a string está terminada
    }

    // Move o ponteiro da linha para depois da vírgula (ou fim de string)
    if (*virgula == ',')
        *linha = virgula + 1;   // Move o ponteiro para o próximo caractere após a vírgula
    else *linha = virgula;      // Se não há vírgula, mantém o ponteiro no fim da string
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


/* Essa função remove logicamente o registro do arquivo, ou seja,
modifica o campo "removido" do registro para '1' */
void remover_registro(FILE *arq, int rrn) {
    if(arq == NULL){
        msg_arquivo_inexistente();  // Não foi possível manipular o arquivo
        return;  // Retorna erro
    }
    int posicao_registro = 1600 + (rrn * 160);  // Calcula a posição do registro com base no RRN
    
    char removido = '1';  // Marca como removido
    int encadeamento = atualiza_topo(arq, rrn);  // O encadeamento recebe o topo da pilha

    fseek(arq, posicao_registro, SEEK_SET);  // Vai para o registro
    // Grava os dados de remoção no arquivo
    fwrite(&removido, sizeof(char), 1, arq);
    fwrite(&encadeamento, sizeof(int), 1, arq);

    // Preenche o restante do registro com '$' (lixo)
    char lixo = '$';
    for (int i = 0; i < 160 - (sizeof(int) + sizeof(char)); i++) // O registro possui tamanho fixo de 160 bytes
        fwrite(&lixo, sizeof(char), 1, arq); 
}

/* As funções seguintes são utilizadas para a busca de diferentes tipos de dados:
inteiro, float, char e string de tamanho variável. Elas foram separadas para facilitar o entendimento
e a busca de cada campo do registro no arquino binário */

/* Essa função realiza a busca de um registro pelos campos inteiros: população ou velocidade
Nessa ordem, são indicadas de 0 a 1 para poder realizar a comparação com os campos do registros lidos.
É utilizada para impressão dos registros (funcionalidade 3) e para remoção (funcionalidade 4)*/
int busca_populacao_velocidade(FILE *arq, int rem_imp, int campo, int valor, int proxRRN){
    if(arq == NULL){
        msg_arquivo_inexistente();  // Não foi possível manipular o arquivo
        return -1;  // Retorna erro
    }

    fseek(arq, 1600, SEEK_SET); // Pula o cabeçalho
    Especie esp;
    int tam_fixo, tam_variavel, i, flag = 0;
    char aux[160];              // Variável auxiliar para leituras

    for(i=0; i< proxRRN; i++){
        tam_fixo = le_camposFixos(arq, &esp);

        if(tam_fixo == -1) return -1;                           // Erro na abertura do arquivo ou leitura dos campos
        if(esp.removido == '1'){                                // Registro removido logicamente - não deve ser impresso
            fread(&aux, sizeof(char), 160 - tam_fixo, arq);     // Pula todos os campos variáveis
            continue;
        }

        // Verifica se a busca foi bem-sucedida
        if(((valor == esp.populacao) && (!campo))               // Campo == 0 (indicativo da busca pela populacao)
        || ((valor == esp.velocidade) && (campo))){             // Campo == 1 (indicativo da busca pela velocidade)
            if(!rem_imp){                                       // Caso queira remover, rem_imp é uma flag = 0
                int rrn = (ftell(arq) - 1600)/160;              // O cursor está no meio do registro, então a divisão gera um arredondamento para baixo
                remover_registro(arq, rrn);
            }
            else if(rem_imp){                                   // Caso queira imprimir, rem_imp é uma flag = 1
                tam_variavel = le_camposVariaveis(arq, &esp);
                imprime_registro(esp);
                fread(&aux, sizeof(char), 160 - tam_fixo - tam_variavel, arq); // Pula para o próximo registro (desconsidera lixo $)
            }
            flag ++;   // Indica que encontrou algum regisro com esse campo
            
        }else 
            fread(&aux, sizeof(char), 160 - tam_fixo, arq);  // Pula todos os campos variáveis caso não tenha encontrado o registro
    }

    if(rem_imp){                                 // Para impressão dos registros
        if(flag == 0){                           // Nenhum registro foi encontrado
            msg_registro_inexistente();
            printf("\n");
        } 

        int pagDisco = ftell(arq) / 1600;       // Indica quantas páginas de disco foram lidas
        if (ftell(arq) % 1600 > 0) 
            pagDisco += 1;                      // Adiciona uma página se houver resto na divisão
        return pagDisco;                        // Retorna a quantidade total de páginas

    }else if(!rem_imp){         // Para a remoção dos registros
        return flag;            // Indica quantos registros foram encontrados e removidos
    }

    return 0;                   // Caso tenha um erro na chamada da função (rem_imp diferente de 0 e 1)
}

/* Essa função realiza a busca de um registro pelo campo de ponto flutuante: tamanho
É utilizada para impressão dos registros (funcionalidade 3) e para remoção (funcionalidade 4)*/
int busca_tamanho(FILE *arq, int rem_imp, float valor, int proxRRN){
    if(arq == NULL){
        msg_arquivo_inexistente();  // Não foi possível manipular o arquivo
        return -1;  // Retorna erro
    }
    fseek(arq, 1600, SEEK_SET);
    Especie esp;
    int tam_fixo, tam_variavel, i, flag = 0;
    char aux[160];
    for(i=0; i<proxRRN; i++){
        tam_fixo = le_camposFixos(arq, &esp);

        if(tam_fixo == -1) return -1;                           // Erro na abertura do arquivo
        if(esp.removido == '1'){                                // Registro removido logicamente - não deve ser impresso
            fread(&aux, sizeof(char), 160 - tam_fixo, arq);     // Pula todos os campos variáveis
            continue;
        }

        if(verifica_float(valor, esp.tamanho)){
            if(!rem_imp){                                           // Caso queira remover, rem_imp é uma flag = 0
                int rrn = (ftell(arq) - 1600)/160;                  // O cursor está no meio do registro. então a divisão gera um arredondamento para baixo
                remover_registro(arq, rrn);                         // Não é necessário mudar o cursor, pois está no início do registro seguinte  
            }
            else if(rem_imp){                                       // Caso queira imprimir, rem_imp é uma flag = 1
                tam_variavel = le_camposVariaveis(arq, &esp);
                imprime_registro(esp);
                fseek(arq, 160 - tam_fixo - tam_variavel, SEEK_CUR); // Pula os caracteres lixo 
            }
            flag ++;            // Indica que encontrou algum regisro com esse campo
        }else
            fread(&aux, sizeof(char), 160 - tam_fixo, arq);         // Pula todos os campos variáveis
    }
    if(rem_imp){                                // Para impressão dos registros
        if(flag == 0){                          // Nenhum registro foi encontrado
            msg_registro_inexistente();
            printf("\n");
        } 

        int pagDisco = ftell(arq) / 1600;       // Indica quantas páginas de disco foram lidas
        if (ftell(arq) % 1600 > 0) 
            pagDisco += 1;                      // Adiciona uma página se houver resto na divisão
        return pagDisco;                        // Retorna a quantidade total de páginas

    }else if(!rem_imp){         // Para a remoção dos registros
        return flag;            // Indica quantos registros foram encontrados e removidos
    }

    return 0;                   // Caso tenha um erro na chamada da função (rem_imp diferente de 0 e 1)
}

/* Essa função realiza a busca de um registro pelo campo de um char: unidade de medida da velocidade
É utilizada para impressão dos registros (funcionalidade 3) e para remoção (funcionalidade 4)*/
int busca_unidadeMedida(FILE *arq, int rem_imp, char valor, int proxRRN){
    if(arq == NULL){
        msg_arquivo_inexistente();
        return 0;
    }
    fseek(arq, 1600, SEEK_SET);
    Especie esp;
    int tam_fixo, tam_variavel, i, flag = 0;
    char aux[160];
    for(i=0; i< proxRRN; i++){
        tam_fixo = le_camposFixos(arq, &esp);
        if(tam_fixo == -1) return -1;                               // Erro na abertura do arquivo
        if(esp.removido == '1'){                                    // Registro removido logicamente - não deve ser impresso
            fread(&aux, sizeof(char), 160 - tam_fixo, arq);         // Pula todos os campos variáveis
            continue;
        }
        if(valor == esp.unidadeMedida){
            if(!rem_imp){                                       // Caso queira remover, rem_imp é uma flag = 0
                int rrn = (ftell(arq) - 1600)/160;              // O cursor está no meio do registro. então a divisão gera um arredondamento para baixo
                remover_registro(arq, rrn);                     // Não é necessário mudar o cursor, pois está no início do registro seguinte  
            }
            else if(rem_imp){                                   // Caso queira imprimir, rem_imp é uma flag = 1
                tam_variavel = le_camposVariaveis(arq, &esp);
                imprime_registro(esp);
                fread(&aux, sizeof(char), 160 - tam_fixo - tam_variavel, arq);  // Pula os caracteres lixo
            }
            flag ++;                // Indica que encontrou algum regisro com esse campo
        }else
            fread(&aux, sizeof(char), 160 - tam_fixo, arq);                     // Pula todos os campos variáveis
    }
    if(rem_imp){                                // Para impressão dos registros
        if(flag == 0){                          // Nenhum registro foi encontrado
            msg_registro_inexistente();
            printf("\n");
        } 

        int pagDisco = ftell(arq) / 1600;       // Indica quantas páginas de disco foram lidas
        if (ftell(arq) % 1600 > 0) 
            pagDisco += 1;                      // Adiciona uma página se houver resto na divisão
        return pagDisco;                        // Retorna a quantidade total de páginas

    }else if(!rem_imp){         // Para a remoção dos registros
        return flag;            // Indica quantos registros foram encontrados e removidos
    }

    return 0;                   // Caso tenha um erro na chamada da função (rem_imp diferente de 0 e 1)
}

/* Essa função realiza a busca de um registro pelo campo de uma string que não pode ser repetida: o nome
É utilizada para impressão dos registros (funcionalidade 3) e para remoção (funcionalidade 4)*/
int busca_variavelUnica(FILE *arq, int rem_imp, char* valor, int proxRRN){
    if(arq == NULL){
        msg_arquivo_inexistente();
        return 0;
    }
    fseek(arq, 1600, SEEK_SET);
    Especie esp;
    int tam_fixo, tam_variavel, i, flag = 0;
    char aux[160];
    for(i=0; i< proxRRN; i++){
        tam_fixo = le_camposFixos(arq, &esp);
        if(tam_fixo == -1) return -1;                               // Erro na abertura do arquivo
        if(esp.removido == '1'){                                    // Registro removido logicamente - não deve ser impresso
            fread(&aux, sizeof(char), 160 - tam_fixo, arq);         // Pula todos os campos variáveis
            continue;
        }
        tam_variavel = le_camposVariaveis(arq, &esp);
        if(!strcmp(valor, esp.nome)){
            if(!rem_imp){                                   // Caso queira remover, rem_imp é uma flag = 0
                int rrn = (ftell(arq) - 1600)/160;          // O cursor está no meio do registro. então a divisão gera um arredondamento para baixo
                remover_registro(arq, rrn);                 // Não é necessário mudar o cursor, pois está no início do registro seguinte  
            }
            else if(rem_imp){                               // Caso queira imprimir, rem_imp é uma flag = 1
                imprime_registro(esp);

            }
            flag ++;                // Indica que encontrou algum regisro com esse campo
            i = proxRRN;            // Break: campo único
        }
        fseek(arq, 160 - tam_fixo - tam_variavel, SEEK_CUR);        // Pula os caracteres lixo 
    }

    if(rem_imp){                                // Para impressão dos registros
        if(flag == 0){                          // Nenhum registro foi encontrado
            msg_registro_inexistente();
            printf("\n");
        } 

        int pagDisco = ftell(arq) / 1600;       // Indica quantas páginas de disco foram lidas
        if (ftell(arq) % 1600 > 0) 
            pagDisco += 1;                      // Adiciona uma página se houver resto na divisão
        return pagDisco;                        // Retorna a quantidade total de páginas

    }else if(!rem_imp){         // Para a remoção dos registros
        return flag;            // Indica quantos registros foram encontrados e removidos
    }

    return 0;                   // Caso tenha um erro na chamada da função (rem_imp diferente de 0 e 1)
}

/* Essa função realiza a busca de um registro pelo campo de uma string que pode ser repetida: especie, habitat, tipo, dieta e alimento.
Nessa ordem, são indicadas de 0 a 4 para poder realizar a comparação com os campos do registros lidos.
É utilizada para impressão dos registros (funcionalidade 3) e para remoção (funcionalidade 4)*/
int busca_variavelNaoUnica(FILE *arq, int rem_imp, int campo, char* valor, int proxRRN){
    if(arq == NULL){
        msg_arquivo_inexistente();
        return 0;
    }
    fseek(arq, 1600, SEEK_SET);
    Especie esp;
    int tam_fixo, tam_variavel, i, flag = 0;
    char aux[160];
    for(i=0; i< proxRRN; i++){
        tam_fixo = le_camposFixos(arq, &esp);
        
        if(tam_fixo == -1) return -1;                               // Erro na abertura do arquivo
        if(esp.removido == '1'){                                    // Registro removido logicamente - não deve ser impresso
            fread(&aux, sizeof(char), 160 - tam_fixo, arq);         // Pula todos os campos variáveis
            continue;
        }
        tam_variavel = le_camposVariaveis(arq, &esp);
        if(((strcmp(valor, esp.especie) == 0) && (campo == 0))
        || ((strcmp(valor, esp.habitat) == 0) && (campo == 1))        
        || ((strcmp(valor, esp.tipo) == 0) && (campo == 2))
        || ((strcmp(valor, esp.dieta) == 0) && (campo == 3))
        || ((strcmp(valor, esp.alimento) == 0) && (campo == 4))){
            if(!rem_imp){                                               // Caso queira remover, rem_imp é uma flag = 0
                int rrn = (ftell(arq) - 1600)/160;                      // O cursor está no meio do registro. então a divisão gera um arredondamento para baixo
                remover_registro(arq, rrn);                             // Não é necessário mudar o cursor, pois está no início do registro seguinte                    
            }
            else if(rem_imp){                                           // Caso queira imprimir, rem_imp é uma flag = 1
                imprime_registro(esp);
                fseek(arq, 160 -tam_fixo -tam_variavel, SEEK_CUR);      // Pula os caracteres lixo 
            }
            flag ++;                    // Indica que encontrou algum regisro com esse campo
        }else{
            fseek(arq, 160 - tam_fixo - tam_variavel, SEEK_CUR);        // Pula os caracteres lixo 
        }
        
    }
    if(rem_imp){                                // Para impressão dos registros
        if(flag == 0){                          // Nenhum registro foi encontrado
            msg_registro_inexistente();
            printf("\n");
        } 

        int pagDisco = ftell(arq) / 1600;       // Indica quantas páginas de disco foram lidas
        if (ftell(arq) % 1600 > 0) 
            pagDisco += 1;                      // Adiciona uma página se houver resto na divisão
        return pagDisco;                        // Retorna a quantidade total de páginas

    }else if(!rem_imp){         // Para a remoção dos registros
        return flag;            // Indica quantos registros foram encontrados e removidos
    }

    return 0;                   // Caso tenha um erro na chamada da função (rem_imp diferente de 0 e 1)
}
