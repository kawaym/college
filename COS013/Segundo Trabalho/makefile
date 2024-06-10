all: entrada
		./pilha < saida.txt
entrada: cleanSaida saida entrada.txt
		-(./saida < entrada.txt) >> saida.txt
lex.yy.c: tradutor.lex
		lex tradutor.lex
saida: lex.yy.c
		g++ -Wall -std=c++17 main.cc -lfl -o saida 
clean:
		rm saida lex.yy.c
cleanSaida:
		-rm saida.txt
unit: saida unit.txt
		./saida < unit.txt