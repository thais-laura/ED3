#include "./cabecalho.h"
#include "./funcionalidades.h"
#include "./funcoesArquivo.h"
#include "./funcoesFornecidas.h"
#include "./manipularVariaveis.h"
#include "./mensagens.h"
#include "./structs.h"

// Leitura de registros a partir do arquivo de entrada CSV, em que serão armazenados no arquivo de saída binário
void funcionalidade1(char *nome_csv, char *nome_bin) {
    FILE *csv = fopen(nome_csv, "r");
    if (csv == NULL) {
        msg_arquivo_inexistente();
        return;
    }

    FILE *bin = fopen(nome_bin, "wb+");             // Nesse modo, cria-se um arquivo binário
    if (bin == NULL) {
        msg_arquivo_inexistente();
        fclose(csv);
        return;
    }

    Cabecalho cab;
    inicia_cabecalho(bin);                          // Inicializa o cabeçalho com valores padrão
    if(atualiza_status(bin, '0'))                   // Define o status do arquivo como inconsistente se antes estiver consistente
        return;

    fseek(bin, 0, SEEK_END);                        // Move para o final do arquivo binário

    char linha[1024];
    Especie reg;
    fgets(linha, sizeof(linha), csv);               // Pular o cabeçalho do CSV

    int contadorRegistros = 0;
    while (fgets(linha, sizeof(linha), csv)) {
        char *linhaPtr = linha;

        // Remove o '\r' e o '\n' no final da linha se existir
        linha[strcspn(linha, "\r\n")] = '\0';

        // Lê cada campo manualmente, lidando com campos vazios
        le_campoCSV(&reg.nome, &linhaPtr);
        le_campoCSV(&reg.dieta, &linhaPtr);
        le_campoCSV(&reg.habitat, &linhaPtr);

        // Campo de população
        char *populacao = NULL;
        le_campoCSV(&populacao, &linhaPtr);
        if (strcmp(populacao, "\0") == 0) {
            reg.populacao = -1;  // Valor nulo
        } else {
            reg.populacao = atoi(populacao);
        }
        free(populacao);  // Libera a memória alocada

        le_campoCSV(&reg.tipo, &linhaPtr);

        // Campo de velocidade
        char *velocidade = NULL;
        le_campoCSV(&velocidade, &linhaPtr);
        if (strcmp(velocidade, "\0") == 0) {
            reg.velocidade = -1;  // Valor nulo
        } else {
            reg.velocidade = atoi(velocidade);
        }
        free(velocidade);

        // Campo de unidade de medida
        char *unidadeMedida = NULL;
        le_campoCSV(&unidadeMedida, &linhaPtr);
        if (strcmp(unidadeMedida, "\0") == 0) {
            reg.unidadeMedida = '$';  // Valor nulo
        } else {
            reg.unidadeMedida = unidadeMedida[0];
        }
        free(unidadeMedida);

        // Campo de tamanho
        char *tamanho = NULL;
        le_campoCSV(&tamanho, &linhaPtr);
        if (strcmp(tamanho, "\0") == 0) {
            reg.tamanho = -1.0;  // Valor nulo
        } else {
            reg.tamanho = atof(tamanho);
        }
        free(tamanho);

        le_campoCSV(&reg.especie, &linhaPtr);
        le_campoCSV(&reg.alimento, &linhaPtr);

        reg.removido = '0';  		// Inicializa como não removido
        reg.encadeamento = -1;  	// Sem encadeamento de registros removidos

        escreve_registro(bin, &reg);  // Escreve o registro no arquivo binário
        contadorRegistros++;

        // Libera as strings alocadas
        free(reg.nome);
        free(reg.dieta);
        free(reg.habitat);
        free(reg.tipo);
        free(reg.especie);
        free(reg.alimento);
    }

    aumenta_RRN(bin, contadorRegistros);	// Redefine o próximo RRN disponível
    atualiza_PagDisco(bin);			// Redefine a quantiadade de páginas de disco 
    atualiza_status(bin, '1');  		// Define o status do arquivo como consistente
    fclose(csv);
    fclose(bin);
    binarioNaTela(nome_bin);  		// Exibe a quantidade de bytes do arquivo
}

// Recuperação de todos os registros do arquivo. Se houver algum campo nulo, não há a sua impressão
void funcionalidade2(char *nome_arq){
    FILE *arq = fopen(nome_arq, "rb+");
    if(arq == NULL){                    // Não foi possível manipular o arquivo
        msg_arquivo_inexistente();
        return;
    }
    if(atualiza_status(arq, '0'))       // Define o status do arquivo como inconsistente se antes estiver consistente
        return;

    Cabecalho cab;
    le_cabecalho(arq, &cab);
    if(cab.proxRRN == 0){               // Ainda não foi inserido algum registro, então não é possível realizar sua impressão
        msg_registro_inexistente();
        atualiza_status(arq, '1');      // Retorna o status para consistente
        fclose(arq);
        return;
    }
    if(cab.nroRegRem - cab.proxRRN == 0){
        msg_registro_inexistente();
        atualiza_status(arq, '1');      // Retorna o status para consistente
        fclose(arq);
        return;
    }

    // Pula para o primeiro registro
    fseek(arq, 1600, SEEK_SET);

    Especie esp;
    int tam_fixo, tam_variavel;
    char aux[160];
    for(int i=0; i<cab.proxRRN; i++){
        tam_fixo = le_camposFixos(arq, &esp);
        if(tam_fixo == -1) i = cab.proxRRN;                     // Erro na abertura do arquivo ou finalizou a leitura do arquivo
        if(esp.removido == '1'){                                // Registro removido logicamente - não deve ser impresso
            fread(&aux, sizeof(char), 160 - tam_fixo, arq);     // Pula todos os campos variáveis
            continue;
        }
        // Registro não removido logicamente
        tam_variavel = le_camposVariaveis(arq, &esp);

        fseek(arq, 160 - tam_fixo - tam_variavel, SEEK_CUR);    // Pula para o próximo registro (desconsidera lixo $)
        imprime_registro(esp);
    }
    // Soma da quantidade de páginas do registro de cabeçalho com a dos registros de dados
    printf("Numero de paginas de disco: %d\n\n", cab.nroPagDisco);
    atualiza_status(arq, '1');                                  // Indicativo que se finalizou a manipulação do arquivo
    fclose(arq);
}

// Recuperação de alguns registros de acordo com a busca por um campo
void funcionalidade3(char *nome_arq, int n){
    FILE *arq = fopen(nome_arq, "rb+");
    if(arq == NULL){                            // Não foi possível manipular o arquivo
        msg_arquivo_inexistente();
        return;
    }
    if(atualiza_status(arq, '0')){              // Define o status do arquivo como inconsistente
        return;
    }
    Cabecalho cab;
    int i;
    le_cabecalho(arq, &cab);
    if(cab.proxRRN == 0){                       // Ainda não foi inserido algum registro
        for(i=0; i<n; i++){
            printf("Busca %d\n", i+1);
            msg_registro_inexistente();
            printf("\n");
            printf("Numero de paginas de disco: 0\n\n");
        }
        atualiza_status(arq, '1');
        fclose(arq);
        return;
    }

    // Pula para o primeiro registro
    fseek(arq, 1600, SEEK_SET);
    CampoValor campos[n];                       // Vetor para armazenar os pares campo-valor
    for(i = 0; i < n; i++) {
        scanf("%s", campos[i].campo);           // Leitura do nome do campo

        // Variáveis inteiras
        if (strcmp(campos[i].campo, "populacao") == 0 || strcmp(campos[i].campo, "velocidade") == 0) {
            campos[i].tipo = TIPO_INT;
            scanf("%d", &campos[i].valor.i);

        // Variável flutuante
        } else if (strcmp(campos[i].campo, "tamanho") == 0) {
            campos[i].tipo = TIPO_FLOAT;
            scanf("%f", &campos[i].valor.f);

        // Variável char
        } else if (strcmp(campos[i].campo, "unidadeMedida") == 0) {
            campos[i].tipo = TIPO_CHAR;
            scanf(" %c", &campos[i].valor.c);

        // Variáveis string
        } else{
            campos[i].tipo = TIPO_STRING;
            scan_quote_string(campos[i].valor.str);
        }
    }

    // Busca os campos inseridos (n vezes) e retorna a quantidade de páginas de disco lidas até encontrar todas as ocorrências
    int pagDisco = 0;
    for(i = 0; i < n; i++) {
        printf("Busca %d\n", i + 1);
        if (campos[i].tipo == TIPO_INT){
            if (strcmp(campos[i].campo, "populacao") == 0)
                pagDisco = busca_populacao_velocidade(arq, 1, 0, campos[i].valor.i, cab.proxRRN);

            else if (strcmp(campos[i].campo, "velocidade") == 0)
                pagDisco = busca_populacao_velocidade(arq, 1, 1, campos[i].valor.i, cab.proxRRN);

        }else if (campos[i].tipo == TIPO_FLOAT)
            pagDisco = busca_tamanho(arq, 1, campos[i].valor.f, cab.proxRRN);

        else if (campos[i].tipo == TIPO_CHAR)
            pagDisco = busca_unidadeMedida(arq, 1, campos[i].valor.c, cab.proxRRN);

        else if (campos[i].tipo == TIPO_STRING) {
            if (strcmp(campos[i].campo, "nome") == 0)
                pagDisco = busca_variavelUnica(arq, 1, campos[i].valor.str, cab.proxRRN);

            else if (strcmp(campos[i].campo, "especie") == 0)
                pagDisco = busca_variavelNaoUnica(arq, 1, 0, campos[i].valor.str, cab.proxRRN);

            else if (strcmp(campos[i].campo, "habitat") == 0)
                pagDisco = busca_variavelNaoUnica(arq,1, 1, campos[i].valor.str, cab.proxRRN);

            else if (strcmp(campos[i].campo, "tipo") == 0)
                pagDisco = busca_variavelNaoUnica(arq, 1, 2, campos[i].valor.str, cab.proxRRN);

            else if (strcmp(campos[i].campo, "dieta") == 0)
                pagDisco = busca_variavelNaoUnica(arq, 1, 3, campos[i].valor.str, cab.proxRRN);

            else if (strcmp(campos[i].campo, "alimento") == 0)
                pagDisco = busca_variavelNaoUnica(arq, 1, 4, campos[i].valor.str, cab.proxRRN);

            else{
                msg_registro_inexistente();
                printf("\n");
            }
        }
        printf("Numero de paginas de disco: %d\n\n", pagDisco);
    }
    atualiza_status(arq, '1');          // Indicativo que se finalizou a manipulação do arquivo
    fclose(arq);
}

// Remoção lógica de registros em um arquivo de entrada binário, com critério de busca definido pelo usuário
void funcionalidade4(char *nome_arq, int n) {
    FILE *arq = fopen(nome_arq, "rb+");
    if(arq == NULL){                           // Não foi possível manipular o arquivo
        msg_arquivo_inexistente();
        return;
    }
    if(atualiza_status(arq, '0'))            	// Define o status do arquivo como inconsistente se antes estiver consistente
        return;

    Cabecalho cab;
    le_cabecalho(arq, &cab);  		// Lê o cabeçalho do arquivo

    if (cab.proxRRN == 0){              	// Se não há registros no arquivo
        msg_registro_inexistente();     	// Mensagem de registro inexistente
        fclose(arq);
        return;
    }

    // Array para armazenar os pares campo-valor para busca e remoção
    CampoValor campos[n];
    int i, contadorRemocoes = 0;  // Contador de remoções bem-sucedidas

    for (i = 0; i < n; i++) {
        // Leitura dos campos e valores de busca
        scanf("%s", campos[i].campo);
        if (strcmp(campos[i].campo, "populacao") == 0 || strcmp(campos[i].campo, "velocidade") == 0){
            campos[i].tipo = TIPO_INT;
            scanf("%d", &campos[i].valor.i);

        }else if(strcmp(campos[i].campo, "tamanho") == 0){
            campos[i].tipo = TIPO_FLOAT;
            scanf("%f", &campos[i].valor.f);

        }else if(strcmp(campos[i].campo, "unidadeMedida") == 0){
            campos[i].tipo = TIPO_CHAR;
            scanf(" %c", &campos[i].valor.c);

        }else{
            campos[i].tipo = TIPO_STRING;
            scan_quote_string(campos[i].valor.str);
        }
    }

    // Laço para remover os registros com base nos critérios
    for (i = 0; i < n; i++) {
        fseek(arq, 0, SEEK_SET);
        if (campos[i].tipo == TIPO_INT){
            if (strcmp(campos[i].campo, "populacao") == 0)
                contadorRemocoes += busca_populacao_velocidade(arq, 0, 0, campos[i].valor.i, cab.proxRRN);
            else if (strcmp(campos[i].campo, "velocidade") == 0)
                contadorRemocoes += busca_populacao_velocidade(arq, 0, 1, campos[i].valor.i, cab.proxRRN);

        }else if (campos[i].tipo == TIPO_FLOAT)
            contadorRemocoes += busca_tamanho(arq, 0, campos[i].valor.f, cab.proxRRN);
        else if (campos[i].tipo == TIPO_CHAR)
            contadorRemocoes += busca_unidadeMedida(arq, 0, campos[i].valor.c, cab.proxRRN);
        else if (campos[i].tipo == TIPO_STRING) {
            if (strcmp(campos[i].campo, "nome") == 0)
                contadorRemocoes += busca_variavelUnica(arq, 0, campos[i].valor.str, cab.proxRRN);

            else if (strcmp(campos[i].campo, "especie") == 0)
                contadorRemocoes += busca_variavelNaoUnica(arq, 0, 0, campos[i].valor.str, cab.proxRRN);

            else if (strcmp(campos[i].campo, "habitat") == 0)
                contadorRemocoes += busca_variavelNaoUnica(arq, 0, 1, campos[i].valor.str, cab.proxRRN);

            else if (strcmp(campos[i].campo, "tipo") == 0)
                contadorRemocoes += busca_variavelNaoUnica(arq, 0, 2, campos[i].valor.str, cab.proxRRN);

            else if (strcmp(campos[i].campo, "dieta") == 0)
                contadorRemocoes += busca_variavelNaoUnica(arq, 0, 3, campos[i].valor.str, cab.proxRRN);

            else if (strcmp(campos[i].campo, "alimento") == 0)
                contadorRemocoes += busca_variavelNaoUnica(arq,0, 4, campos[i].valor.str, cab.proxRRN);

            else {
                msg_registro_inexistente();
                printf("\n");
            }
        }
    }
    atualiza_remocao(arq, contadorRemocoes);		// Redefine a quantidade de registros removidos
    atualiza_status(arq, '1');                  	// Indicativo que se finalizou a manipulação do arquivo

    fclose(arq);
    binarioNaTela(nome_arq);                    	// Exibe o conteúdo atualizado do arquivo binário
}

// Inserção de novos registros no lugar dos registros removidos logicamente a partir do encadeamento da pilha lógica (o topo está definido no cabeçalho)
void funcionalidade5(char *nome_arq, int qtd){
    FILE *arq = fopen(nome_arq, "rb+");
    if(arq == NULL){                            	// Não foi possível manipular o arquivo
        msg_arquivo_inexistente();
        return;
    }
    if(atualiza_status(arq, '0'))               	// Define o status do arquivo como inconsistente se antes estiver consistente
        return;

    Cabecalho cab;
    le_cabecalho(arq, &cab);
    Especie esp;
    for(int i=0; i<qtd; i++){
        // Leitura dos campos do novo registro
        le_e_aloca_string(&esp.nome);
        le_e_aloca_string(&esp.dieta);
        le_e_aloca_string(&esp.habitat);

        esp.populacao = le_inteiro_ou_nulo();
        le_e_aloca_string(&esp.tipo);
        esp.velocidade = le_inteiro_ou_nulo();
        esp.unidadeMedida = le_char_ou_nulo();
        esp.tamanho = le_inteiro_ou_nulo();

        le_e_aloca_string(&esp.especie);
        le_e_aloca_string(&esp.alimento);
        le_cabecalho(arq, &cab);                        // A variável do cabeçalho muda conforme as inserções nos removidos são feitas
        if(cab.nroRegRem == 0 && cab.topo == -1){           // Caso não tenha registros removidos, insere no final do arquivo
            fseek(arq, 0, SEEK_END);
            esp.removido = '0';                             // Registro novo ainda não foi removido
            esp.encadeamento = -1;                          // Não faz parte da pilha lógica
            escreve_registro(arq, &esp);
            aumenta_RRN(arq, 1);                            // A cada inserção no final do arquivo há um aumento no próximo RRN disponível
            atualiza_PagDisco(arq);                         // A cada inserção, pode-se mudar a quantidade de páginas de disco
        }else{
            Especie aux;
            int ultimoRRNrem = cab.topo;                    // Indica o último registro removido, onde será inserido o novo registro
            fseek(arq, 1600 + (ultimoRRNrem * 160), SEEK_SET);
            int tam_fixo = le_camposFixos(arq, &aux);
            int proxRRNrem = aux.encadeamento;              // O penúltimo registro removido será novamente encadeado no topo da pilha lógica
            esp.removido = '0';
            esp.encadeamento = -1;
            fseek(arq, -tam_fixo, SEEK_CUR);                // É necessário voltar para o início do registro
            escreve_registro(arq, &esp);
            atualiza_topo(arq, proxRRNrem);                 // Muda-se o topo da pilha
            atualiza_remocao(arq, -1);                      // Um registro removido foi substituído por outro
        }
        // Liberação da memória alocada dinamicamente
        free(esp.nome);
        free(esp.dieta);
        free(esp.habitat);
        free(esp.alimento);
        free(esp.tipo);
        free(esp.especie);
    }
    atualiza_status(arq, '1');                              	// Indicativo que se finalizou a manipulação do arquivo
    fclose(arq);
    binarioNaTela(nome_arq);					// Exibe a quantidade de bytes do arquivo
}

// Compactação do arquivo binário, eliminando registros removidos, mas o lixo permanece
void funcionalidade6(char *nome_arq) {
    FILE *arq = fopen(nome_arq, "rb+");
    if (arq == NULL) {
        msg_arquivo_inexistente();                  // Mensagem de erro se o arquivo não existir
        return;
    }
    if(atualiza_status(arq, '0'))                   // Define o status do arquivo como inconsistente se estiver consistente
        return;

    Cabecalho cab;
    le_cabecalho(arq, &cab);

    if(cab.nroRegRem == 0){
        atualiza_status(arq, '1');                  // Marca o arquivo como consistente
        atualiza_qttCompacta(arq, 1);		      // Mesmo que não tenha registros para serem removidos fisicamente, a chamada da função ocorreu
        fclose(arq);
        binarioNaTela(nome_arq);                    // Exibe o conteúdo do arquivo binário
        return;
    }

    FILE *tempArq = fopen("temp.bin", "wb+");
    if (tempArq == NULL) {			     // Erro na criação do arquivo temporário e o arquivo original é redefinido como consistente
        msg_arquivo_inexistente();
        atualiza_status(arq, '1');
        atualiza_qttCompacta(arq, 1);
        fclose(arq);
        binarioNaTela(nome_arq);		     // Exibe a quantidade de bytes do arquivo
        return;
    }

    // Escreve um cabeçalho temporário, será atualizado após a compactação
    Cabecalho novoCab = cab;
    novoCab.nroRegRem = 0;                          // Não há registros removidos após a compactação
    grava_cabecalho(tempArq, novoCab);              
    int tam_cabecalho = ftell(tempArq);
    char c = '$';
    for(int i=0; i<(1600 - tam_cabecalho); i++)     // Preenche o restante da página de disco com lixo '$'
        fwrite(&c, sizeof(char), 1, tempArq);

    char registro[160];
    fseek(arq, 1600, SEEK_SET);                     // O cursor está na posição do primeiro registro (após uma página de disco)
    int contador = 0;
    for (int i = 0; i < cab.proxRRN; i++) {
        fread(registro, 160, 1, arq);
        if (registro[0] == '0')                     // Checa se o registro não está marcado como removido
            fwrite(registro, 160, 1, tempArq);      // Copia o registro para o arquivo final
        else   contador++;                          // Contabiliza os registros removidos
    }
    atualiza_status(arq, '1');
    fclose(arq);

    // Não é possível retirar registros mais do que a quantidade de registros removidos
    if(contador > cab.nroRegRem){
        msg_erro_padrao();
        return;
    }
    // Mudanças no cabeçalho
    atualiza_qttCompacta(tempArq, 1);               // Atualiza o contador de compactações
    atualiza_PagDisco(tempArq);                     // A quantidade se manterá ou diminuirá
    diminui_RRN(tempArq, contador);                 // O próximo RRN será menor do que o anterior
    atualiza_status(tempArq, '1');                  // Arquivo consistente
    atualiza_topo(tempArq, -1);                     // Não há mais registros removidos, então não há pilha lógica de removidos
    fclose(tempArq);

    // Substitui o arquivo original pelo compactado (deleta o fornecido e renomeia o temporário com o mesmo nome do anterior)
    remove(nome_arq);
    rename("temp.bin", nome_arq);

    binarioNaTela(nome_arq);			     // Exibe a quantidade de bytes do arquivo
}	
