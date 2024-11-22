#include <iostream>
#include <stdio.h>
#include <string>
#include <algorithm>
#include <set>
#include <cstring>
#include <cstdlib>
#include <list>
#include <stack>
#include <utility>
#include <unordered_map>
#include <queue>
#include <vector>
#include <limits>
#include "./structs.h"
#include "./funcoesGrafo.h"
#include "./funcoesArquivo.h"
#include "./mensagens.h"

// Inicializar a estrutura do grafo
Grafo inicializa_grafo(){
    Grafo grafo;
    grafo.numVertices = 0;  // Primeiramente, não há nenhum vértice no grafo
    return grafo;
}

// Criação do grafo com base no arquivo binário
Grafo cria_grafo(FILE *arq, int proxRRN){
    fseek(arq, 1600, SEEK_SET);
    Especie esp;
    Grafo grafo = inicializa_grafo();
    // Variáveis auxiliares para manipulação do arquivo
    int i, tam_fixo, tam_variavel;
    char aux[160];
    for(i=0; i<proxRRN; i++){           
        tam_fixo = le_camposFixos(arq, &esp);
        if (esp.removido == '0') {                              // Verifica se o registro não está removido
            tam_variavel = le_camposVariaveis(arq, &esp);
            adiciona_verticeEspecie(grafo, esp);                // Acrescenta o registro no grafo
            fread(&aux, sizeof(char), 160 - tam_fixo - tam_variavel , arq);  // Pula o restante do registro
        }else{
            fread(&aux, sizeof(char), 160 - tam_fixo, arq);     // Pula todos os campos variáveis
        }
        // Liberação da memória
        free(esp.nome);
        free(esp.especie);
        free(esp.habitat);
        free(esp.dieta);
        free(esp.alimento);
        free(esp.tipo);
    }
    return grafo;
}

// Acrescenta um novo vértice à lista do grafo durante a leitura do arquivo
void adiciona_verticeEspecie(Grafo &grafo, Especie esp){
    // Cria um vértice de busca para a lista do grafo
    Vertice chave = {esp.nome, "", "", "", "", 0, 0, 0};
    auto it = (grafo.listaVertices).find(chave);
    Vertice v;
    if(it != (grafo.listaVertices).end()){  // Vértice encontrado
        v = *it;
        if((v.especie).empty()){    // Esse vértice foi adicionado como uma aresta primeiro (espécie não pode ser nula no registro)
            v.especie = esp.especie;
            v.habitat = esp.habitat;
            v.dieta = esp.dieta;
            v.tipo = esp.tipo;

            // Remove o vértice anterior e insere o atualizado
            grafo.listaVertices.erase(it);
            grafo.listaVertices.insert(v); 
        }
    }else{     // Vértice não encontrado, deve ser adicionado no grafo
        v.nome = esp.nome;
        v.especie = esp.especie;
        v.habitat = esp.habitat;
        v.dieta = esp.dieta;
        v.tipo = esp.tipo; 
        v.grau_entrada = 0;
        v.grau_saida = 0;
        v.grau = 0;
        (grafo.listaVertices).insert(v);
        grafo.numVertices++;        
    }
    // Cria a aresta de acordo com os dados da espécie
    Aresta a;
    a.nome = esp.alimento;
    a.populacao = esp.populacao;
    if(!a.nome.empty() && a.populacao != -1)
        adiciona_aresta(grafo, v, a);
}

// Acrescenta um novo vértice que é presa de uma espécie e ainda não foi lido o seu registro no arquivo binário
void adiciona_verticeAresta(Grafo &grafo, Aresta a){
    Vertice v_aresta;
    v_aresta.nome = a.nome;
    v_aresta.grau_entrada = 1;  // É uma presa
    v_aresta.grau_saida = 0;
    v_aresta.grau = 1;
    (grafo.listaVertices).insert(v_aresta);
    grafo.numVertices++;
}

// Acrescenta uma nova aresta à lista do vértice
void adiciona_aresta(Grafo &grafo, Vertice v, Aresta a){
    auto it1 = (grafo.listaVertices).find(v);
    auto aux = (v.inicio).insert(a);               // Adiciona a nova aresta
    if(!aux.second)     // Se for falso, então é uma duplicata
        return;

    // Aresta ainda não existe na lista do vértice
    v.grau_saida++;     // É um predador, então aumenta o grau de saída
    v.grau++;
    (grafo.listaVertices).erase(it1);
    (grafo.listaVertices).insert(v);

    // Verificação se a aresta já está na lista de vértices do grafo
    Vertice chave = {a.nome, "", "", "", "", 0, 0, 0};
    auto it2 = (grafo.listaVertices).find(chave);
    if(it2 != (grafo.listaVertices).end()){  // Vértice encontrado
        Vertice arest = *it2;
        (grafo.listaVertices).erase(it2);
        arest.grau_entrada++;               // É uma presa, então aumenta o grau de entrada
        arest.grau++;
        (grafo.listaVertices).insert(arest);
    }else                                   // Vértice não encontrado
        adiciona_verticeAresta(grafo, a);
} 

// Imprime o vértice em conjunto de uma aresta
void imprime_vertice(const Vertice& v) {
    for(Aresta a : v.inicio){
        std::cout << v.nome;
        std::cout << " " <<  v.especie;
        std::cout << " " <<  v.habitat;
        std::cout << " " <<  v.dieta;
        std::cout << " " <<  v.tipo;
        std::cout << " "<< v.grau_entrada << " " << v.grau_saida << " " << v.grau;
        imprime_aresta(a);
    }
}

// Imprime a aresta associada ao vértice
void imprime_aresta(const Aresta& a){ 
    std::cout << " " << a.nome << " " << a.populacao << std::endl;
}

// Imprime todo o grafo
void imprime_grafo(Grafo &grafo) {
    for(Vertice v : grafo.listaVertices) 
        imprime_vertice(v);
}

// Procura todos os predadores da presa em questão, a partir da busca binária da estrutura <set>
// Se existem, os nomes dos predadores são impressos. Se não existirem, imprime a mensagem de erro correspondente
void listaPredadores(Grafo &grafo, const std::string &presaNome) {
    bool encontrado = false;  // Flag para controlar a impressão da vírgula
    bool primeiro = false;

    // Itera por todos os vértices do grafo
    for (const Vertice &v : grafo.listaVertices) {
        // Itera por todas as arestas do vértice atual
        for (const Aresta &a : v.inicio) {
            if (a.nome == presaNome) {  // Verifica se a aresta representa uma relação com a presa
                if (encontrado)        // Se já encontrou um predador anteriormente, imprime uma vírgula
                    std::cout << ", ";

                if(!encontrado){
                    std::cout << presaNome << ": "; // Inicia a impressão com o nome da presa
                    encontrado = true;      // Atualiza a flag para true após encontrar o primeiro predador
                }
                std::cout << v.nome;    // Imprime o nome do predador
            }
        }
    }

    if (!encontrado) {  // Se nenhum predador foi encontrado
        printf("Registro Inexistente.\n\n");
    }else{
        printf("\n\n");  // Imprime uma linha em branco no final
    }
}

/* Função derivada da busca de profunidade para realizar a contagem dos ciclos de um grafo. As estruturas 
utilizadas são: lista de vértices visitados, pilha criada a partir do caminho (para verificar se existe 
uma aresta para um vértice que já está no caminho), lista de pares de vértices já visitados (arestas) a 
fim de não contabilizar laços duas vezes e um inteiro para a contagem dos ciclos do grafo inteiro */
void dfs_qtd(Grafo grafo, Vertice v, std::set<Vertice> &visitados, std::list<Vertice> &pilha, std::set<std::pair<std::string, std::string>> &laco, int &ciclos){
    // printf("Vertice: ");
    // std::cout << v.nome << "\n";
    visitados.insert(v);
    pilha.push_back(v);
    for(Aresta a : v.inicio){
        auto it = grafo.listaVertices.find({a.nome, "", "", "", "", 0, 0, 0});
        Vertice v_aresta = *it; 
        std::list<Vertice>::iterator findIter = std::find(pilha.begin(), pilha.end(), v_aresta);
        if(findIter != pilha.end()){    // Elemento está na pilha!
            if(laco.find({v.nome, a.nome}) == laco.end()){  // Laço ainda não foi contabilizado
                ciclos++;
                if(v.nome == a.nome)
                    laco.insert({v.nome, a.nome});
            }
        // Elemento não visitado ainda ou se tem a possibilidade de uma aresta ser compartilhada por dois ciclos ou mais
        } else if(visitados.find(v_aresta)  == visitados.end() || (v_aresta.grau_entrada != 0 )){     
            dfs_qtd(grafo, v_aresta, visitados, pilha, laco, ciclos);
            pilha.pop_back();
        }
    }
}

/* Função que permite a chamada da busca de profundidade para contagem de ciclos para cada
vértice. Retorna a quantidade de ciclos do grafo em questão */
int conta_ciclos(Grafo& grafo){
    std::set<Vertice> visitados;
    std::list<Vertice> pilha;
    std::set<std::pair<std::string, std::string>> laco;   // Auxiliar para evitar contagem dupla dos laços
    int ciclos = 0;
    // Percorre todos os vértices para a contagem
    for(Vertice v : grafo.listaVertices){
        if(visitados.find(v) == visitados.end()){
            dfs_qtd(grafo, v, visitados, pilha, laco, ciclos);  // Procura os ciclos a partir do vértice v
            pilha.pop_back();
        }
    }
    return ciclos;
}

// Função derivada para encontrar os caminhos (inseridos na pilha) e verificar componente conexo
void dfs_compConexo(Grafo &grafo, Vertice v, std::set<Vertice> &visitados, std::list<std::string> &pilha){
    visitados.insert(v);
    for(Aresta a : v.inicio){
        auto it = grafo.listaVertices.find({a.nome, "", "", "", "", 0, 0, 0});
        Vertice v_aresta = *it; 
        if(visitados.find(v_aresta)  == visitados.end()){     // Elemento não visitado ainda
            dfs_compConexo(grafo, v_aresta, visitados, pilha);
        }
    }
    pilha.push_back(v.nome);    // Insere na pilha caso tenha ele mesmo e seus vértices tenham sido visitados
}

// Realiza a mudança de direção das arestas de um grafo, retornando o seu transposto
Grafo transposicao(Grafo grafo){
    Grafo grafoT;
    grafoT.numVertices = grafo.numVertices;
    Vertice vT;
    for(Vertice aux : grafo.listaVertices){         // Novo predador do grafo transposto (no original é uma presa)
        vT.nome = aux.nome;
        for(Vertice v : grafo.listaVertices){       // Encontra todos os predadores do vértice em questão
            for (const Aresta &a : v.inicio) {
                if (a.nome == aux.nome)             // Verifica se a aresta representa uma relação com a presa
                    vT.inicio.insert({v.nome, 0});
            }
        }
        grafoT.listaVertices.insert(vT);
        vT.inicio.clear();                          // Limpa a lista para o próximo vértice
    }
    return grafoT;
}

// Realiza a contagem dos componentes conexos após a formação da pilha de caminhos e a transposição do grafo
int conta_compConexo(Grafo& grafo){
    std::set<Vertice> visitados;
    std::list<std::string> pilha;
    // A partir do grafo, encontra a árvore mínima do grafo e coloca os vértices na pilha para a segunda parte do algoritmo
    for(Vertice v : grafo.listaVertices){
        if(visitados.find(v) == visitados.end())    // Elemento não visitado ainda
            dfs_compConexo(grafo, v, visitados, pilha);
    }

    Grafo grafoT = transposicao(grafo); // Obtém o grafo transposto 
    visitados.clear();                  // Limpa o conjunto de visitados
    int numComponentes = 0;

    // Percorre a pilha para encontrar os componentes
    while (!pilha.empty()) {
        std::string nome = pilha.back();
        pilha.pop_back();
        // Procura o vértice no grafo transposto para poder percorrer suas arestas depois da busca dfs
        auto it = grafoT.listaVertices.find({nome, "", "", "", "", 0, 0, 0});
        Vertice v = *it;
        if (visitados.find(v) == visitados.end()) {
            dfs_fortemConexo(grafoT, v, visitados); // Visita um novo componente conexo
            numComponentes++;                       
        }
    }
    return numComponentes;
}

/* A partir de um vértice, adiciona à lista "visitados" todos os vértices que possuem
um caminho que os ligam ao vértice de origem */
void dfs_fortemConexo(Grafo grafo, Vertice v, std::set<Vertice> &visitados){
    visitados.insert(v);
    for(Aresta a : v.inicio){
        auto it = grafo.listaVertices.find({a.nome, "", "", "", "", 0, 0, 0});
        Vertice v_aresta = *it; 
        if(visitados.find(v_aresta)  == visitados.end())    // Elemento não visitado ainda
            dfs_fortemConexo(grafo, v_aresta, visitados);
    }
}

/* Para verificar se o grafo é fortemente conexo, é necessário que todos os vértices
sejam conectados por um caminho nos dois sentidos. Logo, inspeciona-se a partir de
um vértice os caminhos do grafo original e do grafo transposto */
bool verifica_fortemConexo(Grafo grafo){
    std::set<Vertice> visitados;                        // Lista de vértices visitados
    auto it = grafo.listaVertices.begin();
    Vertice v = *it;
    dfs_fortemConexo(grafo, v, visitados);              // Começa do primeiro vértice (arbitrário)
    for(Vertice aux : grafo.listaVertices){
        if (visitados.find(aux)  == visitados.end())  // Se algum vértice não foi visitado, o grafo não é fortemente conexo
            return false;   
    }

    Grafo grafoT = transposicao(grafo);                 // Obtém-se o grafo transposto
    visitados.clear();                                  // Limpa a lista de vértices visitados
    auto it_T = grafoT.listaVertices.begin();
    v = *it;
    dfs_fortemConexo(grafoT, v, visitados);             // Começa do primeiro vértice (arbitrário)
    for(Vertice aux : grafoT.listaVertices){
        if (visitados.find(aux)  == visitados.end())    // Se algum vértice não foi visitado, o grafo não é fortemente conexo
            return false;   
    }
    return true;
}

// Por meio do algoritmo de Dijkstra, encontra o menor caminho entre dois vértices, um predador e uma presa
void processaMenorCaminho(Grafo &grafo, const std::string &predador, const std::string &presa) {
    // Mapas para armazenar as distâncias mínimas até cada vértice e o vértice anterior no caminho mínimo
    std::unordered_map<std::string, int> distancias;
    std::unordered_map<std::string, std::string> anterior;

    // Controla quais vértices já foram visitados
    std::set<std::string> visitados;

    // Comparador para a fila de prioridade que considera o menor caminho encontrado até agora
    auto comp = [&](const std::string &a, const std::string &b) {
        return distancias[a] > distancias[b];
    };

    // Gerencia a ordem de visita dos vértices baseada na menor distância, sendo uma fila de prioridade
    std::priority_queue<std::string, std::vector<std::string>, decltype(comp)> fila(comp);

    for (const auto &v : grafo.listaVertices) {
        distancias[v.nome] = std::numeric_limits<int>::max(); // Inicialização das distâncias (infinito)
    }
    distancias[predador] = 0;
    fila.push(predador);

    // Utilizamos o algoritmo de Dijkstra 
    while (!fila.empty()) {
        std::string atual = fila.top();
        fila.pop();
        if (visitados.find(atual) != visitados.end()) continue; // Não conta vértice visitado para evitar duplicações
        visitados.insert(atual);

        auto it = grafo.listaVertices.find(Vertice{atual}); // Encontra o vértice atual no grafo para acessar as arestas
        if (it != grafo.listaVertices.end()) {
            for (const Aresta &a : it->inicio) {
                std::string vizinho = a.nome;
                int peso = a.populacao;

                // Se encontrar um caminho mais curto até o vizinho, atualiza
                if (distancias[vizinho] > distancias[atual] + peso) {
                    distancias[vizinho] = distancias[atual] + peso;
                    anterior[vizinho] = atual;
                    fila.push(vizinho);
                }
            }
        }
    }
    // Impressão do resultado final baseado na distância
    if (distancias[presa] == std::numeric_limits<int>::max())
        std::cout << predador << " " << presa << ": CAMINHO INEXISTENTE" << std::endl;
    else std::cout << predador << " " << presa << ": " << distancias[presa] << std::endl;
}
