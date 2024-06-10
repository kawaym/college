all: saida cleanSaida entrada.txt
	./saida < entrada.txt > saida.txt

saida: lex.yy.c npr.tab.c
	g++ npr.tab.c -o saida -lfl	
	
lex.yy.c: npr.lex
	lex npr.lex
	
counter: npr.y
	bison -Wcounterexamples npr.y

npr.tab.c: npr.y
	bison --verbose npr.y

cleanSaida: saida.txt
	-rm saida.txt && touch saida.txt

clean: cleanSaida
	rm -f lex.yy.c npr.tab.c saida npr.output