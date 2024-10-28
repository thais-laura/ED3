#include "./funcoesFeitas.h"
#define ERRO -1

// Inicializa a struct a fim de evitar comportamentos indefinidos 
void inicializa_especie(Especie *esp){      
    esp->id = 0;
    strcpy(esp->nome, "\0");
    strcpy(esp->nome_cient, "\0");
    esp->populacao = 0;
    strcpy(esp->status, "\0");
    esp->localizacao[0] = 0;
    esp->localizacao[1] = 0;
    esp->impacto_hum = 0;
}

//Preenche campos de string com o $, garantindo que o campo tenha o tamanho fixo
void completa_campo(char* str, int tamanho) {
    int len = strlen(str);
    if (len >= tamanho) {
        str[tamanho - 1] = '\0';                             // Garante que não ultrapassará o limite do buffer
    } else {
        for (int i = len; i < tamanho - 1; i++) {
            str[i] = '$';                                    // Preenche o restante da string com '$'
        }
        str[tamanho - 1] = '\0';                             // Garante que o último caractere seja '\0'
    }
}

//Ve se o ID é 0 e se já existe no arquivo
int verifica_id(FILE* arq, int id) {
    // Verifica se o ID fornecido é inválido (ID = 0)
    if (id == 0) {
        printf("Falha no processamento do arquivo.\n");
        return 1;
    }
 
    // A leitura deve começar no início do arquivo para fazer a verificação
    fseek(arq, 0, SEEK_SET);                                  
    Especie esp_temp;

    // Verifica se o ID fornecido já foi cadastrado no arquivo
    while (fread(&esp_temp, sizeof(Especie), 1, arq) == 1) {
        if (esp_temp.id == id) { 
            printf("Informação já inserida no arquivo.\n");
            return 1;                                         //ID já existe
        }
    }
    return 0;                                                 //ID não encontrado
}

//Verifica se a string é nula
int verifica_string(char* str) {
    return (strcmp(str, "NULO") == 0);
}


//Verifica se a string é NULO SAUDAVEL AMEACADA CRITICA
int verifica_status(char* status) {
    if (strcmp(status, "NULO") == 0 || strcmp(status, "SAUDAVEL") == 0 ||
        strcmp(status, "AMEACADA") == 0 || strcmp(status, "CRITICA") == 0) {
        return 0;                                             // Status válido
    }
    return 1;                                                 // Status inválido
}

//Registra espécies no arquivo binário
int registra_especie(char *nome_arq, int n) {
    FILE *arq = fopen(nome_arq, "w+b");                       // Leitura e escrita a partir do início do arquivo 
    if(arq == NULL){
        printf("Falha no processamento do arquivo\n");
        return ERRO;                    
    }
    Especie esp;  

    for (int i = 0; i < n; i++) {
        // Garante que a struct seja completamente zerada para inicializar o registro
        inicializa_especie(&esp);

        scanf("%d%*c", &esp.id);                             // O '%*c' ignora o caractere '\n' após a entrada

        // Confere se já existe no arquivo ou se é 0 (inválido)
        if (verifica_id(arq, esp.id)) {
            continue;                                        // Segue para a próxima iteração do loop sem registrar
        }

        readline(esp.nome); 
        // Confere se o nome é NULO (inválido)
        if (verifica_string(esp.nome)) {
            printf("Falha no processamento do arquivo.\n");
            continue;                                       // Segue para a próxima iteração do loop sem registrar
        }

        completa_campo(esp.nome, 41);                       //Preenche o restante do campo com '$'

        readline(esp.nome_cient);
        // Confere se o nome científico é NULO (inválido)
        if (verifica_string(esp.nome_cient)) {
            printf("Falha no processamento do arquivo.\n");
            continue;                                       // Segue para a próxima iteração do loop sem registrar
        }
        completa_campo(esp.nome_cient, 61);                 //Preenche o restante do campo com '$'

        scanf("%d%*c", &esp.populacao);                     

        readline(esp.status);  
        // Verifica se o status é válido (NULO, SAUDAVEL, CRITICA ou AMEACADA)
        if (verifica_status(esp.status)) {
            printf("Falha no processamento do arquivo.\n");
            continue;                                       // Segue para a próxima iteração do loop sem registrar       
        }
        completa_campo(esp.status, 9);                      //Preenche o restante do campo com '$'

        // Coordenadas X e Y
        scanf("%f %f%*c", &esp.localizacao[0], &esp.localizacao[1]);

        
        scanf("%d%*c", &esp.impacto_hum);
        // Verifica se o impacto humano é 0, 1, 2, 3 (válidos)
        if (esp.impacto_hum < 0 || esp.impacto_hum > 3) {
            printf("Falha no processamento do arquivo.\n");
            continue;                                       // Segue para a próxima iteração do loop sem registrar
        }

        // Escreve no arquivo binário o registro da espécie
        // É necessário escrever campo por campo a fim não existir "padding" (preenchimento automático de bytes que o compilador insere entre os campos de uma struct para garantir que os dados estejam corretamente alinhados na memória)
        fwrite(&esp.id, sizeof(int), 1, arq);               
        fwrite(esp.nome, sizeof(char), 41, arq);          
        fwrite(esp.nome_cient, sizeof(char), 61, arq);     
        fwrite(&esp.populacao, sizeof(int), 1, arq);        
        fwrite(esp.status, sizeof(char), 9, arq);          
        fwrite(&esp.localizacao[0], sizeof(float), 1, arq); 
        fwrite(&esp.localizacao[1], sizeof(float), 1, arq); 
        fwrite(&esp.impacto_hum, sizeof(int), 1, arq);      

    }
       
    fclose(arq);
    binarioNaTela(nome_arq);
    return 0;
}

// Verifica se o campo não é nulo e retorna o campo sem caracteres lixo ('$')
char * imprime_campo(char *campo){   
    if(strcmp(campo, "\0") == 0){
        printf("Espécie não encontrada\n");                // Indica possível erro ao gravar um conteúdo na variável, já que mostra resquícios da inicialização da struct
        return "-1";
    }else{
        int tamanho = strlen(campo);
        int i;
        char *aux = (char *)malloc(tamanho + 1);           // Variável auxilar para a formatação correta do campo para a impressão

        if (aux == NULL) {
            printf("Falha no processamento do arquivo\n"); // Erro na alocação de memória
            return NULL;  
        }
        for(i=0; i<tamanho && campo[i] != '$'; i++){       // Não insere o caracter '$'
            aux[i] = campo[i];
        }
        aux[i] = '\0';                                     // Inserção do '\0' para a indicação de fim de string
        return aux;
    }
}

// Impressão de todos os campos do registro, utilizando a função auxilar imprime_campo para formatação
int imprime_registro(Especie *esp){   
    if(esp == NULL){                                       // Verifica se a struct existe
        printf("Falha no processamento do arquivo\n");    
        return ERRO;
    }
    // Foi utilizada uma variável auxilar "aux" para a impressão das strings formatadas.
    // Sempre sua alocação dinâmica é liberada

    printf("ID: %d\n", esp->id);
    char * aux = imprime_campo(esp->nome);
    printf("Nome: %s\n", aux); 
    free(aux);

    aux = imprime_campo(esp->nome_cient);
    printf("Nome Científico: %s\n", aux); 
    free(aux);

    if (esp->populacao ==0) printf("População: NULO\n");
    else printf("População: %d\n", esp->populacao);

    aux = imprime_campo(esp->status);
    printf("Status: %s\n", aux);  
    free(aux);

    printf("Localização: (%.2f, %.2f)\n", esp->localizacao[0], esp->localizacao[1]);

    // Para indicar o fim do registro, são usados dois '\n'
    if (esp->impacto_hum ==0) printf("Impacto Humano: NULO\n\n");
    else printf("Impacto Humano: %d\n\n", esp->impacto_hum);

    return 0;
}

// Permite que os registros sejam lidos do arquivo binario e impressos
/* A variável que indica qual das funcionalidades foi chamada é "relatorio":
   para a funcionalidade 2, "relatorio" é dado como 1 e "rrn", como 0 - assim, todos os registros são lidos e impressos pela função "imprime_registro"
   para a funcionalidade 3, "relatorio" é dado como 0 e "rrn", como a entrada pede - desse modo, apenas um registro específico é lido e impresso pela função "imprime_registro"
*/
int le_registro(char *nome_arq, int rrn, int relatorio){  
    FILE *arq = fopen(nome_arq, "rb");
    if(arq == NULL){
        printf("Falha no processamento do arquivo\n");
        return ERRO;
    }

    Especie esp;
    inicializa_especie(&esp); 

    // Para a funcionalidade 3, deseja-se ler apenas um registro dado pelo RRN
    if(relatorio == 0){
        // Primeiramente, é necessário verificar se existe um registro com o "rrn" desejado
        if(fseek(arq, 0, SEEK_END)!= 0){                   // O cursor é deslocado do início para o final do arquivo 
            printf("Falha no processamento do arquivo\n");
        }else{
            int fim = (int) ftell(arq);                    // Indica a posição atual no arquivo (nesse caso, no fim dele), em termos de bytes, a partir do início do arquivo
            if(fim <= rrn * sizeof(Especie)){              
                printf("Espécie não encontrada\n");        // Se o arquivo é menor do que o correspondente do RRN desejado, a espécie não pode ser encontrada
                fclose(arq);
                return 0;
            }    
        }
        if(fseek(arq, 0, SEEK_SET) != 0){                  // O cursor retorna para o início do arquivo a fim de encontrar o registro pelo RRN
            printf("Falha no processamento do arquivo\n"); // Não foi possível retornar para o início do arquivo
            fclose(arq);
            return ERRO;
        }

        // Lê-se os registros até encontrar o com RRN correspondente 
        for(int i=0; i<(rrn+1); i++){
            fread(&esp.id, sizeof(int), 1, arq);
            fread(esp.nome, sizeof(char), 41, arq);
            fread(esp.nome_cient, sizeof(char), 61, arq);
            fread(&esp.populacao, sizeof(int), 1, arq);
            fread(esp.status, sizeof(char), 9, arq);
            fread(&esp.localizacao[0], sizeof(float), 1, arq);
            fread(&esp.localizacao[1], sizeof(float), 1, arq);
            fread(&esp.impacto_hum, sizeof(int), 1, arq);
        }

        // Imprime somente um registro:
        imprime_registro(&esp);
        return 0;
    }
    
    // Nessa parte, a cada vez que um registro é lido, também é impresso
    // Assim, constrói-se o relatório de todos os registros das espécies (funcionalidade 2)
    int resp = 1;                                         // Variável que indica se a impressão do registro foi correta
    while (fread(&esp.id, sizeof(esp.id), 1, arq)){
        fread(esp.nome, sizeof(esp.nome), 1, arq);
        fread(esp.nome_cient, sizeof(esp.nome_cient), 1, arq);
        fread(&esp.populacao, sizeof(esp.populacao), 1, arq);
        fread(esp.status, sizeof(esp.status), 1, arq);
        fread(&esp.localizacao[0], sizeof(esp.localizacao[0]), 1, arq);
        fread(&esp.localizacao[1], sizeof(esp.localizacao[1]), 1, arq);
        fread(&esp.impacto_hum, sizeof(esp.impacto_hum), 1, arq);
        resp = imprime_registro(&esp);  
        if(resp != 0){
            printf("Falha no processamento do arquivo\n");
        }             
    }
   
    fclose(arq);
    return 0;
}

// 
int registra_info(char *nome_arq, int id, int dados[3], int populacao, char *status, int impacto_hum){
    FILE *arq = fopen(nome_arq, "r+b");
    if(arq == NULL){
        printf("Falha no processamento do arquivo\n");
        return 0;
    }

    Especie esp;
    int flag = 0;                                                    // Indicador de algum erro para inserção da nova informação

    while (fread(&esp.id, sizeof(int), 1, arq)) {
        fread(esp.nome, sizeof(char), 41, arq);
        fread(esp.nome_cient, sizeof(char), 61, arq);
        fread(&esp.populacao, sizeof(int), 1, arq);
        fread(esp.status, sizeof(char), 9, arq);
        fread(&esp.localizacao[0], sizeof(float), 1, arq);
        fread(&esp.localizacao[1], sizeof(float), 1, arq);
        fread(&esp.impacto_hum, sizeof(int), 1, arq);

        if (esp.id == id) {                                           // Encontrou o registro correpondente ao ID
            if (dados[0] == 1){                                       // Existe uma tentativa de registrar a população da espécie
                if(esp.populacao != 0){                               // O campo já tem uma informação que não é nula
                    printf("Informação já inserida no arquivo\n");
                    flag = 1;
                }if(populacao == 0){                                  // Dado inválido para inserção    
                    printf("Falha no processamento do arquivo\n");
                    flag = 1;
                }
            }
                
            if (dados[1] == 1){                                       // Existe uma tentatia de registrar o status da espécie
                if(strcmp(esp.status, "NULO") != 0){                  // O campo tem uma informação que não é nula
                    printf("Informação já inserida no arquivo\n");
                    flag = 1;
                }if(verifica_status(status) != 0 || strcmp(status, "NULO") == 0){   // Dado inválido para inserção
                    printf("Falha no processamento do arquivo\n");
                    flag = 1;
                }
            }

            if (dados[2] == 1) {                                      // Existe uma tentativa de registrar o impacto humano da espécie
                if(esp.impacto_hum != 0){                             // O campo tem uma informação que não é nula
                    printf("Informação já inserida no arquivo\n");
                    flag = 1;
                }if (impacto_hum < 1 || impacto_hum > 3) {            // Dado inválido para inserção
                    printf("Falha no processamento do arquivo\n");
                    flag = 1;
                }
            }

            if(flag == 1){                                            // Indicador de algum erro
                fclose(arq);
                binarioNaTela(nome_arq);
                return 0;
            }

            // Se as variáveis passaram por todas as verificações acima, nenhuma informação está duplicada 
            // ou a informação a ser inserida é inválida; logo, é possível realizar a atualização dos campos
            // Novamente, não se realiza um deslocamento do cursor do tamanho da struct devido a "padding"
            fseek(arq, -sizeof(esp.id), SEEK_CUR);
            fseek(arq, -sizeof(esp.nome), SEEK_CUR);
            fseek(arq, -sizeof(esp.nome_cient), SEEK_CUR);
            fseek(arq, -sizeof(esp.populacao), SEEK_CUR);
            fseek(arq, -sizeof(esp.status), SEEK_CUR);
            fseek(arq, -sizeof(esp.localizacao[0]), SEEK_CUR);
            fseek(arq, -sizeof(esp.localizacao[1]), SEEK_CUR);
            fseek(arq, -sizeof(esp.impacto_hum), SEEK_CUR);

            if (dados[0] == 1) {                             // Atualiza população
                esp.populacao = populacao;
            }
            if (dados[1] == 1) {                             // Atualiza status
                completa_campo(status, sizeof(esp.status));  // Insere os caracteres lixo '$'
                strcpy(esp.status, status);
            }
            if (dados[2] == 1) {                             // Atualiza impacto humano
                esp.impacto_hum = impacto_hum;
            }

            // Reescrita de todo o registro com os novos dados
            fwrite(&esp.id, sizeof(int), 1, arq);               
            fwrite(esp.nome, sizeof(char), 41, arq);           
            fwrite(esp.nome_cient, sizeof(char), 61, arq);     
            fwrite(&esp.populacao, sizeof(int), 1, arq);        
            fwrite(esp.status, sizeof(char), 9, arq);          
            fwrite(&esp.localizacao[0], sizeof(float), 1, arq); 
            fwrite(&esp.localizacao[1], sizeof(float), 1, arq); 
            fwrite(&esp.impacto_hum, sizeof(int), 1, arq);      

            fclose(arq);
            binarioNaTela(nome_arq);
            return 0;
        }
    }

    printf("Espécie não encontrada\n");                      // O ID não foi encontrado
    fclose(arq);
    binarioNaTela(nome_arq);
    return 0;
}