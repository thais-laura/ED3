#include "./cabecalho.h"
#include "./cabecalhoArv.h"
#include "./funcionalidades.h"
#include "./funcoesArquivo.h"
#include "./funcoesArvore.h"
#include "./funcoesFornecidas.h"
#include "./manipularVariaveis.h"
#include "./mensagens.h"
#include "./structs.h"


/*A função cria um novo nó na árvore B. Ela configura o nó como folha, inicializa todas as chaves e ponteiros como não
usados/não existentes, e atualiza o cabeçalho do arquivo de índice (próximo RRN do nó disponível). */
void criaNo(FILE *arq, No *novoNo, CabecalhoArv *cabecalho) {
    le_cabecalhoArv(arq, cabecalho);
    novoNo->folha = '1';                    // Inicialmente, todo novo nó é uma folha
    novoNo->nroChavesIndexadas = 0;         // Nenhuma chave indexada ainda
    novoNo->RRNdoNo = cabecalho->RRNproxNo; // Usa o próximo RRN disponível
    int i;

    // Inicializa todos os ponteiros e chaves como -1
    for (i = 0; i < 4; i++) {
        novoNo->P[i] = -1; 
        novoNo->C[i] = -1; 
        novoNo->PR[i] = -1; 
    }
    novoNo->P[4] = -1;

    // Posiciona o arquivo para o lugar correto e escreve o novo nó
    fseek(arq, (novoNo->RRNdoNo +1) * 93, SEEK_SET);  // Utiliza-se + 1 devido à existência do cabeçalho de 93 bytes também
    fwrite(&novoNo->folha, sizeof(char), 1, arq);
    fwrite(&novoNo->nroChavesIndexadas, sizeof(int), 1, arq);
    fwrite(&novoNo->RRNdoNo, sizeof(int), 1, arq);
    for (i = 0; i < 4; i++) { 
        fwrite(&novoNo->P[i], sizeof(int), 1, arq);
        fwrite(&(novoNo->C[i]), sizeof(long int), 1, arq);
        fwrite(&(novoNo->PR[i]), sizeof(long int), 1, arq);
    }
    fwrite(&novoNo->P[i], sizeof(int), 1, arq);
    aumenta_RRNArv(arq, 1);       // Atualiza o próximo RRN disponível na árvore B
}

// Essa função realiza a leitura de um nó da árvore-B
void leNoArv(FILE *arq, No *no){
    fread(&(no->folha), sizeof(char), 1, arq);
    fread(&(no->nroChavesIndexadas), sizeof(int), 1, arq);
    fread(&(no->RRNdoNo), sizeof(int), 1, arq);
    fread(&(no->P[0]), sizeof(int), 1, arq);
    for(int i=0; i < 4; i++){
        fread(&(no->C[i]), sizeof(long int), 1, arq);
        fread(&(no->PR[i]), sizeof(long int), 1, arq);
        fread(&(no->P[i+1]), sizeof(int), 1, arq);
    }
}

// A função grava um nó da árvore B no arquivo de índice de acordo com o RRN do nó.
void gravaNoArvB(FILE *arq, No no) {
    int i;

    if (arq == NULL) {
        msg_arquivo_inexistente();
        return;
    }
    // Posiciona o ponteiro do arquivo no local correto baseado no RRN do nó
    fseek(arq, (no.RRNdoNo +1)* 93, SEEK_SET); 

    // Escreve cada campo do nó no arquivo
    fwrite(&no.folha, sizeof(char), 1, arq);
    fwrite(&no.nroChavesIndexadas, sizeof(int), 1, arq);
    fwrite(&no.RRNdoNo, sizeof(int), 1, arq);
    for (i = 0; i < 5; i++) {
        fwrite(&no.P[i], sizeof(int), 1, arq);
        if (i < 4) {
            fwrite(&no.C[i], sizeof(long int), 1, arq);
            fwrite(&no.PR[i], sizeof(long int), 1, arq);
        }
    }
    // Verificação para garantir que a escrita foi bem-sucedida
    if (ferror(arq)) {
        msg_erro_padrao();
    }
}

// Essa função imprime o nó da árvore B
void imprimeNo(No no){
    printf("Folha: %c\n", no.folha);
    printf("Numero Chaves No: %d\n", no.nroChavesIndexadas);
    printf("RRN do No: %d\n", no.RRNdoNo);
    printf("Ponteiro 0: %d\n", no.P[0]);
    for(int i=0; i < 4; i++){
        printf("Chave %d: %ld\n", i, no.C[i]);
        printf("RRN %d: %ld\n", i, no.PR[i]);
        printf("Ponteiro %d: %d\n", i+1, no.P[i+1]);
    }
}

/* Essa função realiza a comparação entre a chave buscada e a chave do índice
Retorna a "posição" da chave no registro. Caso nenhuma chave corresponder à busca,
retorna -1, indicando que não houve sucesso */
int verifica_chave(int qtd, long int *campos, long int chave, int *pos_chave){
    int i;
    if(qtd==0){    // Nó vazio, então insere na primeira posição
        *pos_chave = 0;
        return -1;
    }
    for(i=0; i<qtd; i++){
        if(campos[i] == chave){ // Chave encontrada
            *pos_chave = i;
            return 1;
        }
        if(campos[i] > chave){ // Encontra onde a chave deve ser inserida no nó
            *pos_chave = i;
            return -1;
        }
    }
    *pos_chave = i;             // Encontra que a chave deve ser inserida no fim do nó
    return -1;
}

// Essa função permite encontrar qual ponteiro deve-se seguir para continuar a busca da chave
// Para isso, é preciso encontrar entre quais duas chaves adjacentes a chave desejada pode estar
int encontra_subarvore(No no, long int chave){
    if(no.folha == '1'){
        return -1;          // Como é um nó folha, não apresenta subárvores
    }
    int i;
    for(i=0; i<(no.nroChavesIndexadas); i++){
        // Como as chaves são ordenadas crescentemente,
        // basta verificar se a chave buscada é menor do que a chave do nó
        if(chave < no.C[i]){    
            return no.P[i];
        }
    }
    return no.P[i];
}

int buscaArv(FILE *arq, int long chave, long int *byteoffset, int *RRN_pagDisco, int *RRN_pai){
    CabecalhoArv cab;
    le_cabecalhoArv(arq, &cab);
    fseek(arq, (cab.noRaiz + 1)*93, SEEK_SET);  // Pula o cabeçalho e vai para a raiz da árvore

    No no;
    leNoArv(arq, &no);
    *RRN_pagDisco = cab.noRaiz; // RRN atual do nó
    *RRN_pai = -1;              // A raiz não possui nó pai
    int pos;
    int flag = verifica_chave(no.nroChavesIndexadas, no.C, chave, &pos);
    if(flag == 1 && pos != -1){     // Chave foi encontrada
        *byteoffset = no.PR[pos];   // Encontra o byteoffset do registro no arquivo de dados
        return 1;
    }

    // Chave não encontrada na raiz
    int subarv;
    while(1){
        subarv = encontra_subarvore(no, chave);    // Verifica para qual subárvore deve partir para a próxima procura da chave
        if(subarv == -1){       // É um nó folha, logo retorna que não encontrou a chave. Porém, sabe-se em qual nó deve ser inserida a chave (RRN_pagDisco)
            *byteoffset = -1;
            return -1;
        }
        // Existe subárvore, logo mudam-se os parâmetros do nó atual e o nó pai
        *RRN_pai = *RRN_pagDisco;
        *RRN_pagDisco = subarv;

        // Como a altura da árvore B geralmente é pequena, não se torna custoso o uso do fseek
        fseek(arq, (subarv + 1)*93, SEEK_SET);  
        leNoArv(arq, &no);

        // Verifica se a chave está no registro da subárvore
        flag = verifica_chave(no.nroChavesIndexadas, no.C, chave, &pos);
        if(flag == 1 && pos != -1){     // Chave encontrada na subárvore
            *byteoffset = no.PR[pos];
            return 1;
        }
    }
}

/* A função é chamada na funcionalidade 7 para criar todo o arquivo de índice (árvore B) 
conforme a leitura do arquivo de dados. Assim, converte o nome da espécie para uma chave 
inteira e há uma tentativa de inserção desta por meio da função insereNo. */
void cria_indice(FILE *arqDados, FILE *arqInd, Cabecalho cabDados, CabecalhoArv cabArv) {
    char aux[160];      // Vetor auxiliar para leitura do restante do registro
    Especie especie;
    long int chave;
    int tam_fixo, pos_antes, pos_depois;                // Inicializa RRN para controlar a posi��o de inser��o
    fseek(arqDados, 1600, SEEK_SET);
    for(int i=0; i<cabDados.proxRRN; i++){              // Byteoffset do registro é justamente a posição do cursor
        tam_fixo = le_camposFixos(arqDados, &especie);
        if (especie.removido == '0') {                  // Verifica se o registro n�o est� removido
            pos_antes = ftell(arqDados);
            le_str(arqDados, &especie.nome);            // É necessário ler somente o nome do registro
            pos_depois = ftell(arqDados);
            chave = converteNome(especie.nome);         // Converte o nome em uma chave long int
            if (chave != -1) {
                insereNo(arqInd, chave, 1600 + 160 * i, &cabArv);
            }
            fread(&aux, sizeof(char), 160 - tam_fixo - (pos_depois-pos_antes) , arqDados);  // Pula o restante do registro
            free(especie.nome);
        }else{
            fread(&aux, sizeof(char), 160 - tam_fixo, arqDados);     // Pula todos os campos variáveis
        }
    }
}

/* A função realiza uma tentativa de inserção de uma chave em algum nó da árvore B. Há quatro  
situações cobertas pela função: árvore vazia (inicia a raiz e insere o nó), chave já inserida 
(não modifica-se o arquivo), inserção da chave em um nó com espaço e inserção da chave em um 
nó completamente cheio (método split com crescimento da árvore bottom-up). */
void insereNo(FILE *arq, long int chave, long int byteoffset, CabecalhoArv *cabecalho) {
    le_cabecalhoArv(arq, cabecalho);
    // Verifica se a árvore está vazia (não há raiz)
    if (cabecalho->noRaiz == -1) {
        // Cria uma nova raiz na árvore
        No novaRaiz;
        criaNo(arq, &novaRaiz, cabecalho);

        // Configura a nova raiz com a chave e RRN_reg
        novaRaiz.C[0] = chave; 
        novaRaiz.PR[0] = byteoffset; 
        novaRaiz.nroChavesIndexadas = 1; // Ajusta o número de chaves indexadas

        // Grava o nó raiz modificado no arquivo
        gravaNoArvB(arq, novaRaiz);
        atualiza_noRaiz(arq, novaRaiz.RRNdoNo);
        return;
    }

    int RRN_pagDisco, encontrado, RRN_pai;
    long int aux;
    // Troca-se byteoffset por aux devido à possível mudança do byteoffset caso a chave não seja encontrada
    // Função importante para encontrar RRN do nó onde a chave será inserida e o RRN do nó pai 
    encontrado = buscaArv(arq, chave, &aux, &RRN_pagDisco, &RRN_pai);

    if (encontrado == -1) { // Chave ainda não inserida
        No no;
        fseek(arq, (RRN_pagDisco+1) * 93, SEEK_SET);  // Posicionamento do cursor para leitura do nó onde será inserida a chave
        leNoArv(arq, &no);

        // Preparar os vetores para possível split
        long int tempC[5] = {-1, -1, -1, -1, -1};   // Temporário para chaves com um espaço extra para a nova chave
        long int tempPR[5] = {-1, -1, -1, -1, -1};  // Temporário para byteoffsets

        // Ordenação das chaves do nó mais a chave a ser inserida
        int pos, inserido = 0;
        for (pos = 0; pos < no.nroChavesIndexadas; pos++) {
            if (inserido == 0 && chave < no.C[pos]) {
                tempC[pos] = chave;
                tempPR[pos] = byteoffset;
                inserido = 1;
            }
            tempC[pos + inserido] = no.C[pos];
            tempPR[pos + inserido] = no.PR[pos];
        }
        if (inserido == 0) { // Se ainda não inseriu porque a chave é a maior
            tempC[pos] = chave;
            tempPR[pos] = byteoffset;
        }

        if (no.nroChavesIndexadas == 4) {   // Inserção em um nó cheio promove split
            // Nó atual fica com as duas primeiras chaves ordenadas
            for(int i=0; i<2; i++){
                no.C[i] = tempC[i];
                no.PR[i] = tempPR[i];
            }
            // Retirar as outras chaves do nó atual
            for(int i=2; i<4; i++){
                no.C[i] = -1;
                no.PR[i] = -1;
            }
            no.nroChavesIndexadas = 2;
            gravaNoArvB(arq, no); // Atualiza o nó atual
            int ponteiros[6] = {-1, -1, -1, -1, -1, -1};    // Como é um nó folha, os ponteiros são nulos
            split(arq, no, tempC, tempPR, ponteiros, cabecalho, RRN_pai);

        } else {
            // Atualiza o nó com os novos vetores, já que possui ainda campos vazios
            for (int i = 0; i < no.nroChavesIndexadas +1; i++){
                no.C[i] = tempC[i];
                no.PR[i] = tempPR[i];
            }
            no.nroChavesIndexadas++;
            gravaNoArvB(arq, no);  // Atualiza o nó atual
        }
    }
    
}

/* A função split implementa a divisão de um nó de uma árvore B devido ao overflow do nó a ser dividido.
Cria-se um nó irmão ao nó atual e a chave promovida é inserida no nó pai. Assim, há alguns casos tratados:
se o nó pai também sofrer overflow, utiliza-se da recursão para tratar dessa questão; também, caso não haja
nó pai, cria-se uma nova raiz. É importante observar que os ponteiros devem ser ajustados a cada interação. */
void split(FILE *arq, No no, long int *chaves, long int *byteoffsets, int *ponteiros, CabecalhoArv *cabecalho, int RRN_pai) {
    // Cria um novo nó para armazenar as chaves que serão movidas
    No novoNo;
    criaNo(arq, &novoNo, cabecalho);
    novoNo.nroChavesIndexadas = 2;          // As duas últimas chaves vão para o novo nó
    novoNo.folha = no.folha;                // Pode ser um nó folha ou não dependendo do nó que sofreu overflow
    // Copia as duas últimas chaves e seus byteoffsets para o novo nó
    for (int i = 0; i < 2; i++) {
        novoNo.C[i] = chaves[i + 3];
        novoNo.PR[i] = byteoffsets[i + 3];
    }
    if(novoNo.folha =='0'){
        // Ajusta os ponteiros para um nó que não é folha
        novoNo.P[0] = ponteiros[3];
        novoNo.P[1] = ponteiros[4];
        novoNo.P[2] = ponteiros[5];
    }
    gravaNoArvB(arq, novoNo);     // Atualiza a árvore

    // A chave promovida será a terceira no vetor ordenado pelas chaves
    long int chavePromovida = chaves[2];
    long int byteoffsetPromovido = byteoffsets[2];

    // Se há um nó pai, atualiza o nó pai com a chave promovida
    if (RRN_pai != -1) {
        No noPai;
        fseek(arq, (RRN_pai + 1) * 93, SEEK_SET);
        leNoArv(arq, &noPai);

        // Cria espaço para a chave promovida no pai
        long int tempC[5];
        long int tempPR[5];
        int tempP[6];
        int inserido = 0, pos = 0;

        // Inserir ordenadamente a chave promovida
        for (int i = 0; i < noPai.nroChavesIndexadas; i++) {
            if (!inserido && chavePromovida < noPai.C[i]) {
                tempC[pos] = chavePromovida;
                tempPR[pos] = byteoffsetPromovido;
                tempP[pos + 1] = novoNo.RRNdoNo;  // Ajusta o ponteiro do novo nó
                inserido = 1;
                pos++;
            }
            tempC[pos] = noPai.C[i];
            tempPR[pos] = noPai.PR[i];
            tempP[pos + 1] = noPai.P[i + 1];
            pos++;
        }
        tempP[0] = noPai.P[0];

        if (!inserido) {
            tempC[pos] = chavePromovida;
            tempPR[pos] = byteoffsetPromovido;
            tempP[pos + 1] = novoNo.RRNdoNo;
        }

        if (noPai.nroChavesIndexadas == 4){ // Se o nó pai também sofrer overflow, realiza-se o split nele
            long int aux1;
            int aux2;
            buscaArv(arq, noPai.C[0], &aux1, &aux2, &RRN_pai); // Atualiza o RRN_pai para o nó em questão

            // Atualiza o nó pai somente com as duas primeiras chaves
            for(int i=0; i<2; i++){
                noPai.C[i] = tempC[i];
                noPai.PR[i] = tempPR[i];
                noPai.P[i] = tempP[i];
            }
            noPai.P[2] = tempP[2];
            // Retira os antigos ponteiros para os nós filhos
            for(int i=2; i<4; i++){
                noPai.C[i] = -1;
                noPai.PR[i] = -1;
                noPai.P[i+1] = -1;
            }
            noPai.nroChavesIndexadas = 2;
            gravaNoArvB(arq, noPai);  // Atualiza o nó no arquivo
            split(arq, noPai, tempC, tempPR, tempP, cabecalho, RRN_pai);  // Realiza o split no nó pai
        }else{
            // Atualiza o nó pai com as novas chaves e ponteiros, já que possui campos disponíveis
            int i;
            for (i = 0; i < noPai.nroChavesIndexadas + 1; i++) {
                noPai.C[i] = tempC[i];
                noPai.PR[i] = tempPR[i];
                noPai.P[i] = tempP[i];
            }
            noPai.P[i] = tempP[i];
            noPai.nroChavesIndexadas++;     // Incrementa a quantidade de chaves presentes no nó
            gravaNoArvB(arq, noPai);  // Atualiza o nó no arquivo
        }

    }else {
        // Se não há nó pai, cria uma nova raiz
        No novaRaiz;
        criaNo(arq, &novaRaiz, cabecalho);
        novaRaiz.folha = '0';                           // A nova raiz não é uma folha
        novaRaiz.nroChavesIndexadas = 1;                // Apenas a chave promovida
        novaRaiz.C[0] = chavePromovida;
        novaRaiz.PR[0] = byteoffsetPromovido;
        novaRaiz.P[0] = cabecalho->noRaiz;              // O nó original
        novaRaiz.P[1] = novoNo.RRNdoNo;                 // O novo nó criado pelo split
        gravaNoArvB(arq, novaRaiz);               // Atualiza o nó no arquivo
        atualiza_noRaiz(arq, novaRaiz.RRNdoNo);   // Como criou outra raiz, atualiza o cabeçalho do arquivo
    }
    
}