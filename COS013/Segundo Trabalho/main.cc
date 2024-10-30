#include <stdio.h>
#include <string>

using namespace std;

extern "C" int yylex();
extern "C" FILE *yyin;

void yyerror(const char *s);
#include "lex.yy.c"

auto p = &yyunput; // Para evitar uma warning de 'unused variable'