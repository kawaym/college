%{
#include <iostream>
#include <string>
#include <vector>
#include <map>

using namespace std;

int row = 0, col = 0;

struct Atributes {
  vector<string> c;

  int row = 0, col = 0;

  void clear() {
    c.clear();
    row = 0;
    col = 0;
  }
};

enum var_types { Let = 1, Const, Var};

struct symbol {
    var_types type;
    int row;
    int col;
};

map< string, symbol > table;

vector<string> declare_var(var_types type, string name, int row, int col);
void check_symbol(string name, bool allowMod);
string get_var(string &name, bool allowMod);
string get_field(string &name);

#define YYSTYPE Atributes

void erro( string msg );
void print( string st );

extern "C" int yylex();
int yyparse();
void yyerror( const char* );

vector<string> concatena( vector<string> a, vector<string> b ) {
  a.insert( a.end(), b.begin(), b.end() );
  return a;
}

std::string cleanQuotes(const std::string& str) {
    if (str.length() >= 2) {
        return str.substr(1, str.length() - 2);
    } else {
        return "";
    }
}

vector<string> operator+( vector<string> a, vector<string> b ) {
  return concatena( a, b );
}

vector<string> operator+( vector<string> a, string b ) {
  a.push_back( b );
  return a;
}

vector<string> operator+( string a, vector<string> b ) {
  return vector<string>{ a } + b;
}

vector<string> solve_address(vector<string> entrada) {
    map<string, int> label;
    vector<string> saida;
    for (int i = 0; i < entrada.size(); i++) {
        if (entrada[i][0] == ':') {
            label[entrada[i].substr(1)] = saida.size();
        }
        else {
            saida.push_back(entrada[i]);
        }
    };

    for (int i = 0; i < saida.size(); i++) {
        if (label.count(saida[i]) > 0) {
            saida[i] = to_string(label[saida[i]]);
        };
    };
    
    return saida;
}

string gera_label( string prefixo ) {
  static int n = 0;
  return prefixo + "_" + to_string( ++n ) + ":";
}

void print( vector<string> codigo ) {
  for( string s : codigo )
    if (s == "\n" || s == "^\n") {
      cout << s;
    }
    // else if(s[0] == '"' || s[0] == '\'') {
    //   cout << cleanQuotes(s) << " ";
    // }
    else {
      cout << s << " ";
    }
    
  cout << endl;  
}

%}

%token	 ID IF ELSE LET CONST VAR PRINT FOR
%token   STRING NUM
%token   DOUBLE_EQUAL
%token   PLUS_EQUAL DOUBLE_PLUS

%right '='
%nonassoc GT ST
%left   ELSE '-' '+'
%left  '*' '/' '%'

%%

S   : CMDs { print(solve_address($1.c + "."));}
    ;

CMDs: CMDs CMD  { $$.c = $1.c + $2.c; }
    |           { $$.clear();}
    ;

CMD : CMD_LET ';' { $$.c = $$.c + "\n"; }
    | CMD_IF 
    | '{' CMDs '}'  { $$.c = $2.c; }
    | Expr ';'  { $$.c = $1.c + "^\n"; }
    ;

CMD_LET : LET LET_VARs { $$.c = $2.c;  }
        ;

LET_VARs: LET_VAR ',' LET_VARs { $$.c = $1.c + $3.c; }
        | LET_VAR 
        ;

LET_VAR : ID { $$.c = declare_var( Let, $1.c[0], $1.row, $1.col ); }
        | ID '=' '{' '}' { $$.c = declare_var(Let, $1.c[0],  $1.row, $1.col) + $1.c + "{}" + "=" + "^"; }
        | ID '=' '[' ']' { $$.c = declare_var(Let, $1.c[0],  $1.row, $1.col) + $1.c + "[]" + "=" + "^"; }
        | ID '=' Expr { $$.c = declare_var( Let, $1.c[0], $1.row, $1.col ) + $1.c + $3.c + "=" + "^"; }
        ;
        
CMD_IF  : IF '(' Expr ')' CMD ELSE CMD 
        {
            string lbl_true = gera_label( "lbl_true" );
            string lbl_fim_if = gera_label( "lbl_fim_if" );
            string definicao_lbl_true = ":" + lbl_true;
            string definicao_lbl_fim_if = ":" + lbl_fim_if;
            $$.c = $3.c + lbl_true + "?" + $7.c + lbl_fim_if 
            + "#" + definicao_lbl_true + $5.c + definicao_lbl_fim_if;
        }
LVALUE : ID 
       ;
       
LVALUEPROP : Expr '[' Expr ']' {$$.c = $1.c + $3.c; }
           | Expr '.' LVALUE  {$$.c = $1.c + $3.c;}
           ;

Expr  : LVALUE '=' '{' '}' { check_symbol( $1.c[0], true ); $$.c = $1.c + "{}" + "="; }
      | LVALUE '=' Expr  { check_symbol( $1.c[0], true ); $$.c = $1.c + $3.c + "="; }
      | LVALUEPROP '=' Expr { $$.c = $1.c + $3.c + "[=]";}
      | Expr ST Expr { $$.c = $1.c + $3.c + $2.c; }
      | Expr GT Expr { $$.c = $1.c + $3.c + $2.c; }
      | Expr '+' Expr { $$.c = $1.c + $3.c + $2.c; }
      | Expr '-' Expr { $$.c = $1.c + $3.c + $2.c; }
      | Expr '*' Expr { $$.c = $1.c + $3.c + $2.c; }
      | Expr '/' Expr { $$.c = $1.c + $3.c + $2.c; }
      | Expr '%' Expr { $$.c = $1.c + $3.c + $2.c; }
      | Expr DOUBLE_EQUAL Expr { $$.c = $1.c + $3.c + $2.c; }
      | LVALUE PLUS_EQUAL Expr { $$.c = $1.c + get_var($1.c[0], true) + $3.c + "+" + "="; }
      | DOUBLE_PLUS LVALUE { vector<string> vec; $$.c = vec + get_var($2.c[0], true) + "1" + "+" + "="; }
      | LVALUE DOUBLE_PLUS { vector<string> vec; $$.c = vec + get_var($1.c[0], true) + "^" + get_var($1.c[0], true) + "1" + "+" + "="; }
      | NUM
      | STRING 
      | LVALUE  { vector<string> vec; $$.c = vec + get_var($1.c[0], false); } 
      | LVALUEPROP 
      | '(' Expr ')' { $$.c = $2.c; }
      | '(' '{' '}' ')' { $$.c = vector<string>{"{}"}; }
      | '{' '}' {$$.c = vector<string>{"{}"}; }
      | '[' ']' {$$.c = vector<string>{"[]"}; }
      ;

%%

#include "lex.yy.c"

vector<string> declare_var( var_types type, string name, int row, int col) {
    /* cerr << "insert_symbol( " << type << ", " << name << ", " << row << ", " << col << ")" << endl; */

    if (table.count(name) == 0) {
        table[name] = symbol{type, row, col};
        return vector<string>{ name + "&" };
    }
    else if (type == Var && table[name].type == Var) {
        table[name] = symbol{type, row, col};
        return vector<string>{};
    }
    else {
        cerr << "Erro: a variável '" << name << "' ja foi declarada na linha " << table[name].row + 1 << "." << endl;
        exit(1);
    }
}

string get_var (string &name, bool allowMod) {
  check_symbol(name, allowMod);
  string s = name + '@';
  return s;
}

string get_field (string &name){
  string s = name + "[@]";
  return s;
}

void check_symbol(string name, bool allowMod) {
    if (table.count(name) > 0) {
        if (allowMod && table[name].type == Const) {
            cerr << "Variavel '" << name << "' não pode ser modificada." << endl;
            exit(1);
        }
    }
    else {
        cerr << "Erro: a variável '" << name << "' não foi declarada." << endl;
        exit(1);
    }

}

void yyerror( const char* st ) {
    cerr << st << endl;
    cerr << "Perto de: " << yytext << ":" << row + 1 << ":" << col <<endl;
    exit(1);
}

int main() {
    yyparse();
  
    return 0;
}
