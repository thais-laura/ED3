#include "./funcoesFeitas.h"
#define ERRO -1

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
        str[tamanho - 1] = '\0'; // Garante que não ultrapassará o limite do buffer
    } else {
        for (int i = len; i < tamanho - 1; i++) {
            str[i] = '$';  // Preenche o restante da string com '$'
        }
        str[tamanho - 1] = '\0';  // Garante que o último caractere seja o terminador nulo
    }
}

//Ve se o ID é 0 e se já existe no arquivo
int verifica_id(FILE* arq, int id) {
    // Verifica se o ID fornecido é inválido, já que não pode ter ID == 0
    if (id == 0) {
        printf("Falha no processamento do arquivo.\n");
        return 1;
    }

    Especie esp_temp;  //Variavel temporária que armazena espécies lidas do arquivo
    fseek(arq, 0, SEEK_SET); //Garante que a leitura sera feita no início do arquivo p fazer a verificacao

    // Percorre o arquivo verificando se já existe uma espécie com o mesmo ID
    while (fread(&esp_temp, sizeof(Especie), 1, arq) == 1) {
        if (esp_temp.id == id) { //Compara o ID do arquivo com o inserido por ultimo na entrada
            printf("Informação já inserida no arquivo.\n");
            return 1; //ID já existe
        }
    }
    return 0; //ID não encontrado
}

//Verifica se uma string é nula
int verifica_string(char* str) {
    return (strcmp(str, "NULO") == 0);
}


//Verifica se uma string é NULO SAUDAVEL AMEACADA CRITICA
int verifica_status(char* status) {
    if (strcmp(status, "NULO") == 0 || strcmp(status, "SAUDAVEL") == 0 ||
        strcmp(status, "AMEACADA") == 0 || strcmp(status, "CRITICA") == 0) {
        return 0;  //Valido
    }
    return 1;  //Invalido
}

//Registra espécies no arquivo bin
int registra_especie(char *nome_arq, int n) {
    FILE *arq = fopen(nome_arq, "w+b"); // leitura e escrita no início do arquivo 
    if(arq == NULL){
        printf("Falha no processamento do arquivo\n");
        return ERRO;                    // mudar pra 0 depois, pq já tem uma mensagem de erro
    }
    Especie esp;  //Variavel que armazena os dados da especia q sera registrada

    for (int i = 0; i < n; i++) {
        //Garante que a estrutura especie seja completamente zerada, para inicializar o registro
        inicializa_especie(&esp);

        //Le ID
        scanf("%d%*c", &esp.id);  // O '%*c' ignora o caractere '\n' após a entrada

        //Chama a função do ID, para ver se já existe no arquivo ou se é 0
        if (verifica_id(arq, esp.id)) {
            continue;  //Pula para o próximo registro sem gravar o ID -- para essa iteração do for e vai para i+1
        }

        //Le o nome
        readline(esp.nome);  //Le a string
        if (verifica_string(esp.nome)) {
            printf("Falha no processamento do arquivo.\n");
            continue;  //Pula para o proximo registro se o nome for inválido
        }

        completa_campo(esp.nome, 41);  //Preenche o restante do campo com $

        //Le nome científico
        readline(esp.nome_cient);  //Le a string
        if (verifica_string(esp.nome_cient)) {
            printf("Falha no processamento do arquivo.\n");
            continue;  //Pula para o próximo registro se o nome científico for inválido
        }
        completa_campo(esp.nome_cient, 61);  //Preenche o campo restante com $

        //Le a população q pode ser nula
        scanf("%d%*c", &esp.populacao);

        //Le o status e verifica se é válido
        readline(esp.status);  //Le a string do status
        if (verifica_status(esp.status)) {
            printf("Falha no processamento do arquivo.\n");
            continue;  //Pula para o próximo registro se o status for inválido
        }
        completa_campo(esp.status, 9);  //Preenche p campo restante com $

        //Le as coord X e Y
        scanf("%f %f%*c", &esp.localizacao[0], &esp.localizacao[1]);

        //Le o impacto humano e verifica se é válido
        scanf("%d%*c", &esp.impacto_hum);
        if (esp.impacto_hum < 0 || esp.impacto_hum > 3) {
            printf("Falha no processamento do arquivo.\n");
            continue;  //Pula para o próximo registro se for inválido
        }

        //Escreve no arquivo binário o registro da espécie, a sua estrutura!!
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

// essa função verifica se o campo tem alguma coisa (nao nulo) e retira os $$ na hora de imprimir
char * imprime_campo(char *campo){   
    if(strcmp(campo, "\0") == 0){
        printf("Espécie não encontrada\n"); //Erro ao gravar na variável -- resquícios da inicialização
        return "-1";
    }else{
        int tamanho = strlen(campo);
        
        int i;
        char *aux = (char *)malloc(tamanho + 1);
        if (aux == NULL) {
            printf("Falha no processamento do arquivo\n"); // mudar erro?
            return NULL;  
        }
        for(i=0; i<tamanho && campo[i] != '$'; i++){
            aux[i] = campo[i];
        }
        aux[i] = '\0';
        return aux;
    }
}

// essa função imprime todo o registro de acordo com o formato pedido no trabalho.
// chama a função imprime_campo no caso de strings --> manipular campos nulos e os caracteres lixo $
int imprime_registro(Especie *esp){   
    if(esp == NULL){
        printf("Falha no processamento do arquivo\n");
        return ERRO;
    }
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

    if (esp->impacto_hum ==0) printf("Impacto Humano: NULO\n\n");
    else printf("Impacto Humano: %d\n\n", esp->impacto_hum);

    return 0;
}
//essa função serve para as funcionalidades 2 e 3
// ler todos os registros (rrn = 0 e relatorio = 1) ou le somente um registro depois do fseek (relatorio = 0)
//usa fseek pra pular para o registro desejado
int le_registro(char *nome_arq, int rrn, int relatorio){   // file * arq
    FILE *arq = fopen(nome_arq, "rb");
    if(arq == NULL){
        printf("Falha no processamento do arquivo\n");
        return ERRO;
    }

    Especie esp;
    inicializa_especie(&esp); 
    // se entrar um rrn maior do que o possível, deve ter uma saída de erro
    if(relatorio == 0){
        if(fseek(arq, 0, SEEK_END)!= 0){
            printf("Falha no processamento do arquivo\n");
        }else{
            int fim = (int) ftell(arq);   // retorna em qual byte vc está do arquivo
            if(fim <= rrn * sizeof(Especie)){
                printf("Espécie não encontrada\n");
                fclose(arq);
                return 0;
            }    
        }
        if(fseek(arq, 0, SEEK_SET) != 0){                        // fseek ou rewind
            printf("Falha no processamento do arquivo\n");
            fclose(arq);
            return ERRO;
        }
        
        for(int i=0; i<(rrn+1); i++){
            int pos = (int)ftell(arq);
            fread(&esp.id, sizeof(int), 1, arq);
            fread(esp.nome, sizeof(char), 41, arq);
            fread(esp.nome_cient, sizeof(char), 61, arq);
            fread(&esp.populacao, sizeof(int), 1, arq);
            fread(esp.status, sizeof(char), 9, arq);
            fread(&esp.localizacao[0], sizeof(float), 1, arq);
            fread(&esp.localizacao[1], sizeof(float), 1, arq);
            fread(&esp.impacto_hum, sizeof(int), 1, arq);
        }
        
        imprime_registro(&esp);
        return 0;
    }
        
    int resp = 1;
    char c;
    while (!feof(arq)){
        fread(&esp.id, sizeof(esp.id), 1, arq);
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
        }             // se resp = 0, deu tudo certo para imprimir o registro
        if(fread(&c, 1, 1, arq) == 1){               // conseguiu ler, então...
            fseek(arq, -1, SEEK_CUR);                // ainda não é o fim do arquivo, precisa voltar um
        }
    }
   

    fclose(arq);
    return 0;
}


int registra_info(char *nome_arq, int id, int dados[3], int populacao, char *status, int impacto_hum){
    FILE *arq = fopen(nome_arq, "r+b");
    if(arq == NULL){
        printf("Falha no processamento do arquivo\n");
        return 0;
    }

    Especie esp;
    int flag = 0;
    char c;

    while (fread(&esp.id, sizeof(int), 1, arq)) {
        fread(esp.nome, sizeof(char), 41, arq);
        fread(esp.nome_cient, sizeof(char), 61, arq);
        fread(&esp.populacao, sizeof(int), 1, arq);
        fread(esp.status, sizeof(char), 9, arq);
        fread(&esp.localizacao[0], sizeof(float), 1, arq);
        fread(&esp.localizacao[1], sizeof(float), 1, arq);
        fread(&esp.impacto_hum, sizeof(int), 1, arq);
        if (esp.id == id) {
            if (dados[0] == 1 && esp.populacao != 0){
                printf("Informação já inserida no arquivo\n");
                flag = 1;
            }
                
            if (dados[1] == 1 && strcmp(esp.status, "NULO") != 0){
                printf("Informação já inserida no arquivo\n");
                flag = 1;
            }
            if (dados[2] == 1 && esp.impacto_hum != 0) {
                printf("Informação já inserida no arquivo\n");
                flag = 1;
            
            }
            if(flag == 1){
                fclose(arq);
                binarioNaTela(nome_arq);
                return 0;
            }
            // Se chegou aqui, nenhuma informação estava duplicada e podemos atualizar
        
            fseek(arq, -sizeof(esp.id), SEEK_CUR);
            fseek(arq, -sizeof(esp.nome), SEEK_CUR);
            fseek(arq, -sizeof(esp.nome_cient), SEEK_CUR);
            fseek(arq, -sizeof(esp.populacao), SEEK_CUR);
            fseek(arq, -sizeof(esp.status), SEEK_CUR);
            fseek(arq, -sizeof(esp.localizacao[0]), SEEK_CUR);
            fseek(arq, -sizeof(esp.localizacao[1]), SEEK_CUR);
            fseek(arq, -sizeof(esp.impacto_hum), SEEK_CUR);

            if (dados[0] == 1) { // Atualiza população
                esp.populacao = populacao;
            }
            if (dados[1] == 1) { // Atualiza status
                completa_campo(status, sizeof(esp.status));
                strcpy(esp.status, status);
            }
            if (dados[2] == 1) { // Atualiza impacto humano
                esp.impacto_hum = impacto_hum;
            }

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
            
        }else if(fread(&c, sizeof(char), 1, arq)==1){
            fseek(arq, -(sizeof(char)), SEEK_CUR);
        }
    }
    printf("Espécie não encontrada\n");
    //printf("NAO CONSEGUIU ESCREVER NO ARQUIVO\n");
    fclose(arq);
    binarioNaTela(nome_arq);
    return 0;
}