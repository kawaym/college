all: criaSaida
	./interpretador < saida.txt

saida: lex.yy.c npr.tab.c
	g++-7 npr.tab.c -o saida -lfl	

criaSaida: saida cleanSaida entrada.txt
	./saida < entrada.txt > saida.txt

lex.yy.c: npr.lex
	lex npr.lex
	
counter: npr.y
	bison -Wcounterexamples npr.y --report-file=counter.txt -r counterexamples

npr.tab.c: npr.y
	bison --verbose npr.y

cleanSaida: 
	-rm saida.txt && touch saida.txt

clean: cleanSaida
	-rm -f lex.yy.c npr.tab.c saida npr.output

teste:
	./interpretador < saida.txt