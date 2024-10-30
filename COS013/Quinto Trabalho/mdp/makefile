
all: lex.yy.mdp.o var_object.cc mdp.h mdp.cc
	 g++-7 -Wall -std=c++17 -lfl lex.yy.mdp.o -o interpretador 
		
lex.yy.mdp.o: lex.yy.mdp.c 
	g++-7 -Wall -std=c++17 -c lex.yy.mdp.c 
	
lex.yy.mdp.c: mdp.l 
	lex -o lex.yy.mdp.c mdp.l 
	
