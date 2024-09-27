---
title: "Teoria dos Grafos: Trabalho 1"
author: [Kaway Marinho, Lucas Ikuhara]
date: "2024-09-25"
---

# Introdução

O objetivo deste trabalho é desenvolver uma biblioteca capaz de ler e manipular dados em estruturas de grafos. Sua implementação foi feita utilizando a linguagem Rust, devido a combinação de sua alta performance e ferramentas de desenvolvimento modernas. 

# Design da Biblioteca

Nesta seção, busca-se explicar algumas das decisões tomadas durante o desenvolvimento da biblioteca, quanto ao seu design e organização. 

## Estruturas de Dados

As principais estruturas de dados necessárias para o desenvolvimento da biblioteca foram implementadas utilizando structs. Desta forma, foi utilizado um struct para o Grafo, um para o vértice, um para aresta, e um para nós da árvore de busca. A definição destes structs se encontra no arquivo `graph.rs`.

## Leitura dos Grafos

A funcionalidade de leitura de grafos de arquivos de texto foi implementada no arquivo `parser.rs`. Neste arquivo se encontra a função read_graph, que recebe o caminho do arquivo a ser lido e gera um struct do tipo Graph.

## Representações

A função de leitura de arquivos gera um struct em que o grafo é representado via lista de adjacências. Esta escolha foi feita devido ao menor gasto de memória observado utilizando esta representação. 

É também possível representar o grafo como matriz, usando a função create_adjacency_matrix.

## Funções do Grafo

Algumas funções foram implementadas para operar sobre os structs `Graph`. Vale notar que Rust não é uma linguagem orientada a objetos, e portanto não suporta a ideia de métodos propriamente ditos. Entretanto, é possível obter funcionalidade análoga associando funções a tipos específicos. Abaixo, exploraremos as funções julgadas mais importantes.

### add_edge

Adiciona uma aresta, conectando dois vértices. Caso um dos vértices não exista, ele será criado automaticamente.

### get_degree

Conjunto de funções relacionadas a grau. A função base get_degree é a base das outras, e calcula o grau de um nó, simplesmente contando suas conexões. As funções adicionais get_degree_maximum, get_degree_minimum, get_degree_average e get_degree_median foram implementadas executando a função get_degree base em todos os nós do grafo, e acumulando o resultado em uma variável. A única exceção é no cálculo da mediana, em que salvou-se todos os graus em um vetor, para que pudessem ser contados depois.

### bfs e dfs

As funções bfs e dfs foram implementadas para realizar busca em largura e profundidade respectivamente. As implementações foram em grande parte iguais as apresentadas em sala, porém com uma otimização: a informação de vértice visitado foi armazenada nos próprios vértices, pois provou-se mais rápido fazê-lo desta forma. Ao inicio de uma nova busca no entanto, é necessário percorrer os vértices desmarcando-os como visitados.

Além disso, foram adicionadas as funções display_bfs e display_dfs, que geram uma representação visual em texto da busca. Abaixo se encontra um exemplo da visualização:

```sh
vértice: 1
  -> nível: 0
  -> pai: 0
vértice: 2
  -> nível: 1
  -> pai: 1
vértice: 3
  -> nível: 2
  -> pai: 2
```

### calculate_distance

Calcula a distância entre dois nós utilizando o algoritmo BFS, porém o encontrar do nó destino como critério de parada.

### calculate_diameter

Calcula o diâmetro do grafo. Esta função foi implementada utilizando calculate_distance, calculando a distância de cada nó para cada um dos outros nós do grafo. Ao final, a maior distância é dita o diâmetro do grafo.

### calculate_approximate_diameter

Calcula o diâmetro aproximado do grafo. Esta função foi implementada utilizando BFS a partir de todos os nós do grafo. É escolhido o nó que gerou a árvore com mais níveis. A partir dele, uma nova BFS é feita. O diâmetro aproximado será o maior nível da árvore gerada por essa BFS.

### calculate_connected_components

Encontra os componentes conexos existentes no grafo. Para isso, a partir de um nó é feita uma DFS. Ao final de sua execução observa-se os nós não visitados. A partir do primeiro dos nós não visitados, é executada outra DFS. O processo se repete até que não existam nós não visitados. Ao final, a árvore de busca de cada DFS conterá o conjunto de vértices de cada componente conexo.

# Estudos de Caso

Os casos de estudo descritos no trabalho foram implementados no arqui

# Resultados

| Representação | N   | Tempo (s) | Memória (MB) |
| ------------- | --- | --------- | ------------ |
| Matrix        | 1   | 32        | 2            |

