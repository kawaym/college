%{ 
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <map>

using namespace std;

string lexema;
int token;

void A();
void E();
void E_linha();
void T();
void T_linha();
void F();
void V();
void VS();
void PT();
void casa (int);

enum { _ID = 256, _CTE_INT, _CTE_FLOAT, _STRING, _PRINT, _MAX, _POWER, _DTOS};

map<int, string>  token_names = {
  {_ID, "IDENTIFIER"},
  {_CTE_INT, "INTEGER CONSTANT"},
  {_CTE_FLOAT, "FLOATING CONSTANT"},
  {_STRING, "STRING"},
  {_PRINT, "PRINT FUNCTION"},
  {_MAX, "MAX FUNCTION"},
  {_POWER, "POWER FUNCTION"},
  {_DTOS, "DTOS FUNCTION"}
};
  
%}

WS [ \n\r\t]+
D [0-9]
L [a-zA-Z_]


INT {D}+
FLOAT {D}+(\.{D}+)?([Ee][+\-]?{D}+)?
ID ([$]|{L}|"_")*({D})*

CARACTERES_ESCAPE (\\a)|(\\b)|(\\e)|(\\f)|(\\n)|(\\r)|(\\t)|(\\y)|(\\)|(\\?)|(\\nnn)|(\\)|(\/)
CARACTERES_ESPECIAIS \!|\,|\:
ASPAS_DUPLAS_ESCAPADA (\\\")|(\"\")
ASPAS_SIMPLES_ESCAPADA (\\\')|(\'\')

ABRE_COMENTARIO (\/\*)
FECHA_COMENTARIO (\*\/)
COMENTARIO "/*"([^*]|(\*+[^*/]))*\*+"/"|(\/\/.*)

ASPAS_DUPLAS \"({L}|{D}|{WS}|!|\"|{ASPAS_SIMPLES_ESCAPADA}|{CARACTERES_ESCAPE}|{COMENTARIO}|{CARACTERES_ESPECIAIS})*\"
ASPAS_SIMPLES \'({L}|{D}|{WS}|!|\"|{ASPAS_SIMPLES_ESCAPADA}|{CARACTERES_ESCAPE}|{COMENTARIO})*\'


STRING {ASPAS_DUPLAS}|{ASPAS_SIMPLES}

%%

{WS}      { }
{INT}     { lexema = yytext; return _CTE_INT; }
{FLOAT}   { lexema = yytext; return _CTE_FLOAT; }
"print"   { lexema = yytext; return _PRINT; }
"max"     { lexema = yytext; return _MAX; }
"dtos"    { lexema = yytext; return _DTOS; }

{STRING}  { lexema = yytext; return _STRING; }

"^"       { lexema = yytext; return _POWER; }

{ID}      { lexema = yytext; return _ID; }

.         { lexema = yytext; return yytext[0]; }

%%

int next_token() {
  return yylex();
}

string findToken (int token) {
  if (token_names.find(token) != token_names.end()) {
    return token_names[token];
  }
  else {
    string r;

    r = token;
    return r;
  }
}

void matchToken (int expected_token) {
  if (token == expected_token) {
    token = next_token();
  }
  else {
    cout << "Esperado " << findToken(expected_token) << " , encontrado: " << findToken(token) << endl;
    exit(1);
  }
}

void A() {
  string temp = lexema;
  matchToken(_ID);
  cout << temp << ' ';
  matchToken('=');
  E();
  printf("= ^ ");
}

void E() {
  T();
  E_linha();
}

void E_linha() {
    switch(token) {
    case '+': matchToken('+'); T(); printf("+ "); E_linha(); break;
    case '-': matchToken('-'); T(); printf("- "); E_linha(); break;
  }
}

void PT() {
  E();
  switch(token) {
    case ',': matchToken(','); PT();
  }
}

void T() {
  F();
  T_linha();
}

void T_linha() {
  switch(token) {
    case '*': matchToken('*'); F(); printf("* "); T_linha(); break;
    case '/': matchToken('/'); F(); printf("/ "); T_linha(); break;
  }
}

void F() {
  switch(token) {
    case _ID: {
      string temp = lexema;
      matchToken(_ID);
      if (token == '(') {
        matchToken('(');
        PT();
        cout << temp << " # ";
        matchToken(')');
      }
      else {
        cout << temp << ' ' << "@ ";
      }
      break;
    }
    case _CTE_INT: {
      string temp = lexema;
      matchToken(_CTE_INT);
      cout << temp << ' ';
      break;
    }
    case _CTE_FLOAT: {
      string temp = lexema;
      matchToken(_CTE_FLOAT);
      cout << temp << ' ';
      break;
    }
    case _STRING: {
      string temp = lexema;
      matchToken(_STRING);
      cout << temp << ' ';
      break;
    }
    case '(': {
      matchToken('(');
      E();
      matchToken(')'); 
      break;
    }
    case '-': {
      matchToken('-'); printf("0 "); F(); printf("- "); break;
    }
    case '+': {
      matchToken('+'); F(); break;
    }
    case _MAX: {
      matchToken(_MAX);
      matchToken('(');
      PT();
      matchToken(')');
      cout << "max " << "# ";
      break;
    }
    case _DTOS: {
      matchToken(_DTOS);
      matchToken('(');
      PT();
      matchToken(')');
      cout << "dtos # ";
      break;
    }
  }
  if (token == '!') {
        matchToken('!');
        cout << "fat # ";
      }
  if (token == _POWER) {
    matchToken(_POWER);
    T();
    cout << "power # ";
  }
}

void P() {
  string temp = lexema;
  matchToken(_PRINT);
  E();
  cout << temp << ' ';
  cout << "# ";
  matchToken(';');
}

void V() {
  A();
  matchToken(';');
}

void VS() {
  switch(token) {
    case _ID:
      V();
      VS();
      break;
    case _PRINT:
      P();
      VS();
      break;

  }
}

int main()
{
    token = next_token();
    VS();
    cout << endl;
    return 0;
}