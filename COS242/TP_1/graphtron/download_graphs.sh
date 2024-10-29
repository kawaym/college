#!/bin/sh
cd data
curl https://www.cos.ufrj.br/~daniel/grafos-2023/data/grafo_W_1.txt.gz | gunzip -c > grafo_W_1.txt
curl https://www.cos.ufrj.br/~daniel/grafos-2023/data/grafo_W_2.txt.gz | gunzip -c > grafo_W_2.txt
curl https://www.cos.ufrj.br/~daniel/grafos-2023/data/grafo_W_3.txt.gz | gunzip -c > grafo_W_3.txt
curl https://www.cos.ufrj.br/~daniel/grafos-2023/data/grafo_W_4.txt.gz | gunzip -c > grafo_W_4.txt
curl https://www.cos.ufrj.br/~daniel/grafos-2023/data/grafo_W_5.txt.gz | gunzip -c > grafo_W_5.txt
curl https://www.cos.ufrj.br/~daniel/grafos-2021/data/rede_colaboracao.txt -o rede_colaboracao.txt
curl https://www.cos.ufrj.br/~daniel/grafos-2021/data/rede_colaboracao_vertices.txt -o rede_colaboracao_vertices.txt
