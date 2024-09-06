#include "./funcoesFeitas.h"
#define ERRO -1
#define REG 131

void inicializar_especie(Especie *esp){       // memset -- inicializa tudo com 0
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
    int len = strlen(str);  //Calculo do comprimento da string
    for (int i = len; i < tamanho; i++) {
        str[i] = '$';  //Preenche o restante da string com '$'
    }
    str[tamanho - 1] = '\0';  //Garante que o último caractere seja o terminador nulo
}

//Ve se o ID é 0 e se já existe no arquivo
int verifica_id(FILE* arq, int id) {
    // Verifica se o ID fornecido é inválido, já que não pode ter ID == 0
    if (id == 0) {
        printf("Falha no processamento do arquivo.\n");
        return 1;
    }

    Especie especie_temp;  //Variavel temporária que armazena espécies lidas do arquivo
    fseek(arq, 0, SEEK_SET); //Garante que a leitura sera feita no início do arquivo p fazer a verificacao

    // Percorre o arquivo verificando se já existe uma espécie com o mesmo ID
    while (fread(&especie_temp, sizeof(Especie), 1, arq) == 1) {
        if (especie_temp.id == id) { //Compara o ID do arquivo com o inserido por ultimo na entrada
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
void registra_especie(char *nome_arq, int n) {
    FILE *arq = fopen(nome_arq, "r+b"); // leitura e escrita no início do arquivo 
    if(arq == NULL){
        printf("Falha no processamento do arquivo\n");
        printf("Arq == NULL\n");
        return ERRO;                    // mudar pra 0 depois, pq já tem uma mensagem de erro
    }
    Especie especie;  //Variavel que armazena os dados da especia q sera registrada

    for (int i = 0; i < n; i++) {
        //Garante que a estrutura especie seja completamente zerada, para inicializar o registro
        memset(&especie, 0, sizeof(Especie));

        //Le ID
        scanf("%d%*c", &especie.id);  // O '%*c' ignora o caractere '\n' após a entrada

        //Chama a função do ID, para ver se já existe no arquivo ou se é 0
        if (verifica_id(arq, especie.id)) {
            continue;  //Pula para o próximo registro sem gravar o ID -- para essa iteração do for e vai para i+1
        }

        //Le o nome
        readline(especie.nome);  //Le a string
        if (verifica_string(especie.nome)) {
            printf("Falha no processamento do arquivo.\n");
            continue;  //Pula para o proximo registro se o nome for inválido
        }

        completa_campo(especie.nome, 41);  //Preenche o restante do campo com $

        //Le nome científico
        readline(especie.nome_cient);  //Le a string
        if (verifica_string(especie.nome_cient)) {
            printf("Falha no processamento do arquivo.\n");
            continue;  //Pula para o próximo registro se o nome científico for inválido
        }
        completa_campo(especie.nome_cient, 61);  //Preenche o campo restante com $

        //Le a população q pode ser nula
        scanf("%d%*c", &especie.populacao);

        //Le o status e verifica se é válido
        readline(especie.status);  //Le a string do status
        if (verifica_status(especie.status)) {
            printf("Falha no processamento do arquivo.\n");
            continue;  //Pula para o próximo registro se o status for inválido
        }
        completa_campo(especie.status, 9);  //Preenche p campo restante com $

        //Le as coord X e Y
        scanf("%f %f%*c", &especie.localizacao[0], &especie.localizacao[1]);
        //localizacao pode negativa???

        // if (especie.localizacao[0] == 0 && especie.localizacao[1] == 0) {
        //     printf("Falha no processamento do arquivo.\n");
        //     continue;  //Pula para o próximo registro se a localização for inválida
        // }

        //Le o impacto humano e verifica se é válido
        scanf("%d%*c", &especie.impacto_hum);
        if (especie.impacto_hum < 0 || especie.impacto_hum > 3) {
            printf("Falha no processamento do arquivo.\n");
            continue;  //Pula para o próximo registro se for inválido
        }

        //Escreve no arquivo binário o registro da espécie, a sua estrutura!!
        fwrite(&especie, sizeof(Especie), 1, arq);
    }

    fclose(arq);
}

//essa função serve para as funcionalidades 2 e 3
// ler todos os registros (rrn = 0 e relatorio = 1) ou le somente um registro depois do fseek (relatorio = 0)
//usa fseek pra pular para o registro desejado
int le_registro(char nome_arq[TAM], int rrn, int relatorio){   // file * arq
    printf("Nome do arquivo: %s\n", nome_arq);                                    // retirar
    FILE *arq = fopen(nome_arq, "rb");
    if(arq == NULL){
        printf("Falha no processamento do arquivo\n");
        printf("Arq == NULL\n");
        return ERRO;
    }

    // se entrar um rrn maior do que o possível, deve ter uma saída de erro
    if(fseek(arq, 0, SEEK_END)!= 0){
        printf("Falha no processamento do arquivo\n");
        printf("Não conseguiu ir para o fim do arquivo\n");
    }else{
        int fim = ftell(arq);   // retorna em qual byte vc está do arquivo
        if(fim <= rrn *REG){
            printf("Espécie não encontrada\n");
            fclose(arq);
            return 0;
        }    
    }
    
    if(fseek(arq, rrn * REG, SEEK_SET) != 0){                        // fseek ou rewind
        printf("Falha no processamento do arquivo\n");
        printf("Não conseguiu voltar para o início do arquivo\n");
        fclose(arq);
        return ERRO;
    }   

    Especie *esp = (Especie *) malloc(sizeof(Especie));
    if (esp == NULL) {
        printf("Erro ao alocar memória para esp\n");
        fclose(arq);
        return ERRO;
    }
    inicializar_especie(esp);        // depois, se virmos que a leitura e a gravacao dos campos nessa variavel deu certo, podemos retirar essa inicializacao
    int resp = 1;
    char c;
    do{
        fread(&(esp->id), 4, 1, arq);
        if(esp->id == 0){
            printf("Erro: id inválido\n");
        }
        fread(esp->nome, 41, 1, arq);  // Aqui não precisa de &, pois "nome" já é um array
        fread(esp->nome_cient, 61, 1, arq);  
        fread(&(esp->populacao), 4, 1, arq);
        fread(esp->status, 9, 1, arq);
        fread(esp->localizacao, 8, 1, arq);
        fread(&(esp->impacto_hum), 4, 1, arq);
        resp = imprimir_registro(esp);               // se resp = 0, deu tudo certo para imprimir o registro
        // comentar melhor
        if(fread(&c, 1, 1, arq) == 1){               // conseguiu ler, então...
            fseek(arq, -1, SEEK_CUR);                // ainda não é o fim do arquivo, precisa voltar um
        }
    }while (resp == 0 && relatorio && !feof(arq));   

    /*
    para o relatório:
        rrn=0 e relatorio = 1 --> loop vai ate o fim do arquivo
    para buscar um registro:
        rrn= ? e relatório = 0 --> loop vai apenas uma vez
    */

    free(esp);
    fclose(arq);
    return 0;
}

// essa função verifica se o campo tem alguma coisa (nao nulo) e retira os $$ na hora de imprimir
char * imprime_campo(char *campo){   
    if(strcmp(campo, "\0") == 0){
        printf("Espécie não encontrada\n"); //Erro ao gravar na variável -- resquícios da inicialização
        printf("Na funcao imprimir_campo, algum campo não foi gravado --> string ainda = barra0\n"); //retirar isso dps
        return "-1";
    }else{
        int tamanho = strlen(campo);
        int i;
        char *aux = (char *)malloc(tamanho + 1);
        if (aux == NULL) {
            printf("Erro de alocação de memória\n"); // mudar erro?
            printf("na funcao imprimir_campo, a variavel aux = NULL\n"); //retirar
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
        printf("Na funcao imprimir_registro, a struct está vazia\n");  //retirar isso depois
        return ERRO;
    }
    printf("ID: %d\n", esp->id);
    printf("Nome: %s\n", imprimir_campo(esp->nome)); 
    printf("Nome científico: %s\n", imprimir_campo(esp->nome_cient)); 
    
    if (esp->populacao ==0) printf("População: NULO\n");
    else printf("População: %d\n", esp->populacao);

    printf("Status: %s\n", imprimir_campo(esp->status));  
    printf("Localização: (%.2f,%.2f)\n", esp->localizacao[0], esp->localizacao[1]);

    if (esp->impacto_hum ==0) printf("Impacto Humano: NULO\n");
    else printf("Impacto Humano: %d\n\n", esp->impacto_hum);

    return 0;
}

int registra_info(char *nome_arq, int id, int dados[3], int populacao, char *status, int impacto_hum){
    FILE *arq = fopen(nome_arq, "r+b"); // leitura e escrita no início do arquivo 
    if(arq == NULL){
        printf("Falha no processamento do arquivo\n");
        printf("Arq == NULL\n");
        return ERRO;                    // mudar pra 0 depois, pq já tem uma mensagem de erro
    }
    int aux, flag = 0;
    char c;

    while(aux != id && !feof(arq)){
        fseek(arq, REG, SEEK_CUR);            // REG - x
        fread(&aux, 4, 1, arq);
        if(fread(&c, 1, 1, arq) == 1){               // conseguiu ler, então...
            fseek(arq, -1, SEEK_CUR);                // ainda não é o fim do arquivo, precisa voltar um
        }else{
            flag = 1;
        }
    }
    Especie especie_temp;
    while (flag == 0 && !feof(arq)) {
        fread(&especie_temp, sizeof(Especie), 1, arq) == 1;
        if (especie_temp.id == id) { //Compara o ID do arquivo com o inserido por ultimo na entrada
            flag = 1;
        }if(fread(&c, 1, 1, arq) == 1){               // conseguiu ler, então...
            fseek(arq, -1, SEEK_CUR); 
        }
    }

    if(flag == 0){                                 //nao encontrou o id e o arquivo todo já foi lido
        printf("Espécie não encontrada\n");
        printf("O id não foi encontrado para inserir novas informações\n");
        return ERRO;                               // mudar pra 0 depois, pq já tem uma mensagem de erro
    }

    if(dados[0]==1){                              // indica a populacao                            // vai indicar para pular menos
        fseek(arq, 102, SEEK_CUR);
        //inserir
        
    }if(dados[1] == 1){                            // indica o status 
        if(dados[0] == 1){                         //status está em seguida da populacao
            //inserir
        }else{
            fseek(arq, 106, SEEK_CUR);
            //inserir
        }
    }if(dados[2]==1){                              // indica o impacto humano
        if(dados[0]==1){
            if(dados[1]==1){                       // nao pula mais nada 
                fseek(arq, 8, SEEK_CUR);
                //inserir
            }else{                                 // pula o status
                fseek(arq, 17, SEEK_CUR);
                //inserir
            }
        }else{
            fseek(arq, 123, SEEK_CUR);
        }
    }
    
    fclose(arq);
    binarioNaTela(nome_arq);

    return 0;
}