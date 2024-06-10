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
  int counter = 0;
  vector<string> default_value;

  void clear() {
    default_value.clear();
    c.clear();
    row = 0;
    col = 0;
    counter = 0;
  }
};

enum var_types { Let = 1, Const, Var};
map<var_types, string> vatTypeName = {
  {Let, "let"},
  {Const, "const"},
  {Var, "var"}
};

struct symbol {
    var_types type;
    int row;
    int col;
    int n_args;
};

vector<map<string, symbol>> table = {map<string, symbol>{}};

vector<string> functions;

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
      // cout << saida[i] << " " << i << endl;
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
  for( string s : codigo ){
    if (s == "\n" || s == "^") {
      cout << s << endl;;
    }

    else if (s == ".") {
      cout << endl << s << endl;
    }
    // else if(s[0] == '"' || s[0] == '\'') {
    //   cout << cleanQuotes(s) << " ";
    // }
    else {
      cout << s << " ";
    }
  }
  cout << endl;  
}

%}

%token	 ID IF ELSE LET CONST VAR FOR FUNCTION ASM RETURN
%token   STRING NUM V_TRUE V_FALSE
%token   DOUBLE_EQUAL DELIMITER
%token   PLUS_EQUAL DOUBLE_PLUS

%right '='
%nonassoc GT ST
%left   ELSE '-' '+'
%left  '*' '/' '%'

%%

FIM : S {$$.c = solve_address($1.c + "." + functions); print($$.c);}

S   : CMDs
    ;

CMDs: CMDs CMD  { $$.c = $1.c + $2.c; }
    |           { $$.clear();}
    ;

CMD : CMD_LET DELIMITER { $$.c = $$.c; }
    | CMD_CONST DELIMITER { $$.c = $$.c; }
    | CMD_VAR DELIMITER { $$.c = $$.c; }
    | CMD_IF 
    | '{' EMPILHA_TABLE CMDs DESEMPILHA_TABLE '}'  { $$.c = "<{" + $3.c + "}>"; }
    | Expr DELIMITER  { $$.c = $1.c + "^"; }
    | CMD_FUNC DELIMITER
    | CMD_FUNC
    | CMD_FOR DELIMITER
    | Expr ASM DELIMITER {$$.c = $1.c + $2.c + "^"; }
    | RETURN Expr DELIMITER {$$.c = $2.c + "'&retorno'" + "@" + "~"; } 
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

CMD_CONST : CONST CONST_VARs { $$.c = $2.c;  }
        ;

CONST_VARs: CONST_VAR ',' CONST_VARs { $$.c = $1.c + $3.c; }
        | CONST_VAR 
        ;

CONST_VAR : ID { $$.c = declare_var( Let, $1.c[0], $1.row, $1.col ); }
        | ID '=' '{' '}' { $$.c = declare_var(Const, $1.c[0],  $1.row, $1.col) + $1.c + "{}" + "=" + "^"; }
        | ID '=' '[' ']' { $$.c = declare_var(Const, $1.c[0],  $1.row, $1.col) + $1.c + "[]" + "=" + "^"; }
        | ID '=' Expr { $$.c = declare_var( Const, $1.c[0], $1.row, $1.col ) + $1.c + $3.c + "=" + "^"; }
        ;



CMD_VAR : VAR VAR_VARs { $$.c = $2.c;  }
        ;

VAR_VARs: VAR_VAR ',' VAR_VARs { $$.c = $1.c + $3.c; }
        | VAR_VAR 
        ;

VAR_VAR : ID { $$.c = declare_var( Let, $1.c[0], $1.row, $1.col ); }
        | ID '=' '{' '}' { $$.c = declare_var(Var, $1.c[0],  $1.row, $1.col) + $1.c + "{}" + "=" + "^"; }
        | ID '=' '[' ']' { $$.c = declare_var(Var, $1.c[0],  $1.row, $1.col) + $1.c + "[]" + "=" + "^"; }
        | ID '=' Expr { $$.c = declare_var( Var, $1.c[0], $1.row, $1.col ) + $1.c + $3.c + "=" + "^"; }
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
        | IF '(' Expr ')' CMD 
        {
          string lbl_true = gera_label( "lbl_true" );
          string lbl_fim_if = gera_label( "lbl_fim_if" );
          string definicao_lbl_true = ":" + lbl_true;
          string definicao_lbl_fim_if = ":" + lbl_fim_if;
          $$.c = $3.c + lbl_true + "?" + lbl_fim_if + "#" + definicao_lbl_true + $5.c + definicao_lbl_fim_if;
          //  $$.c = $3.c + lbl_true + "?" + $5.c + definicao_lbl_true ;
        }
        ;

CMD_FOR : FOR '(' PRIM_E DELIMITER Expr DELIMITER Expr ')' CMD
        {
          string lbl_fim_for = gera_label("fim_for");
          string lbl_condicao_for = gera_label("condicao_for");
          string lbl_comando_for = gera_label("comando_for");
          string def_fim_for = ":" + lbl_fim_for;
          string def_condicao_for = ":" + lbl_condicao_for;
          string def_comando_for = ":" + lbl_comando_for;

          $$.c = $3.c + def_condicao_for + $5.c + lbl_comando_for + "?" + lbl_fim_for + "#"
          + def_comando_for + $9.c + $7.c + "^" + lbl_condicao_for + "#" + def_fim_for;
        }

PRIM_E  : CMD_LET
        | Expr { $$.c = $1.c + "^"; }
        ;

EMPILHA_TABLE : { table.push_back( map<string, symbol>{}); }
              ;

DESEMPILHA_TABLE : { table.pop_back(); }

CMD_FUNC : FUNCTION ID { declare_var(Var, $2.c[0], $2.row, $2.col); }
          '(' EMPILHA_TABLE PARAMETERS ')' '{' CMDs '}'
          {
            string lbl_function_address = gera_label("func_" + $2.c[0]);
            string def_function_address = ":" + lbl_function_address;

            $$.c = $2.c + "&" + $2.c + "{}" + "=" + "'&funcao'" + lbl_function_address
            + "[=]" + "^";
            functions = functions + def_function_address + $6.c + $9.c + "undefined"
            + "@" + "'&retorno'" + "@" + "~";
            table.pop_back();
          } 
         /* | FUNCTION ID { declare_var(Var, $2.c[0], $2.row, $2.col); }
          '(' EMPILHA_TABLE PARAMETERS ')' '{' CMDs RETURN Expr ';' '}'
          {
            string lbl_function_address = gera_label("func_" + $2.c[0]);
            string def_function_address = ":" + lbl_function_address;

            $$.c = $2.c + "&" + $2.c + "{}" + "=" + "'&funcao'" + lbl_function_address
            + "[=]" + "^";
            functions = functions + def_function_address + $6.c + $9.c + "undefined"
            + $10.c + "'&retorno'" + "@" + "~"
            + "@" + "'&retorno'" + "@" + "~";
            table.pop_back();
          } */
         ;   

PARAMETERS  : PARAMs 
            | { $$.c.clear(); }
            ;

PARAMs    : PARAMs ',' PARAM
            { $$.c = $1.c + $3.c + "&" + $3.c + "arguments" + "@" + to_string($1.counter) +"[@]" + "=" + "^" ;
              $$.counter = $$.counter + $1.counter + $3.counter;      
            }
          | PARAM { $$.c = $1.c + "&" + $1.c + "arguments" + "@" + to_string($1.counter) + "[@]" + "=" + "^"; $$.counter = $1.counter + 1;}
          ;  

PARAM :   ID  {$$.c = $1.c; $$.counter = 0; $$.default_value.clear(); declare_var(Let, $1.c[0], $1.row, $1.col); }
      |   ID '=' Expr { $$.c = $1.c; $$.counter = 0; 
                        $$.default_value = $3.c; 
                        declare_var(Let, $1.c[0],  $1.row, $1.col); } 
      ;
ARGUMENTS : ARGs
          | {$$.c.clear();}
          ;
ARGs : ARGs ',' ARG {$$.c = $1.c + $3.c; $$.counter = $1.counter + $3.counter; }
     | ARG {$$.c = $1.c; $$.counter = $1.counter; }
     ;

ARG : Expr {$$.c = $1.c; $$.counter = 1; }

LVALUE : ID 
       ;
       
LVALUEPROP : Expr '[' Expr ']' {$$.c = $1.c + $3.c + "[@]"; }
           | Expr '[' Expr ']' '=' Expr {$$.c = $1.c + $3.c + $6.c + "[=]" ; } 
           /* | Expr '.' LVALUE  {$$.c = $1.c + $3.c;} */
           | Expr '.' LVALUE '=' Expr {$$.c = $1.c + $3.c + $5.c + "[=]" ;}
           | Expr '.' LVALUE { $$.c = $1.c + $3.c + "[@]"; } 
           ;

Expr  : LVALUE '=' '{' '}' { check_symbol( $1.c[0], true ); $$.c = $1.c + "{}" + "="; }
      | LVALUE '=' Expr { check_symbol( $1.c[0], true ); $$.c = $1.c + $3.c + "="; }
      | Expr ST Expr { $$.c = $1.c + $3.c + $2.c; }
      | Expr GT Expr { $$.c = $1.c + $3.c + $2.c; }
      | Expr '+' Expr { $$.c = $1.c + $3.c + $2.c; }
      | Expr '-' Expr { $$.c = $1.c + $3.c + $2.c; }
      | Expr '*' Expr { $$.c = $1.c + $3.c + $2.c; }
      | Expr '/' Expr { $$.c = $1.c + $3.c + $2.c; }
      | Expr '%' Expr { $$.c = $1.c + $3.c + $2.c; }
      | Expr DOUBLE_EQUAL Expr { $$.c = $1.c + $3.c + $2.c; }
      | LVALUE PLUS_EQUAL Expr { $$.c = $1.c + get_var($1.c[0], true) + "@" + $3.c + "+" + "="; }
      | DOUBLE_PLUS LVALUE { vector<string> vec; $$.c = vec + get_var($2.c[0], true) + "@" + "1" + "+" + "="; }
      | LVALUE DOUBLE_PLUS { vector<string> vec; $$.c = vec + get_var($1.c[0], true) + "@"; }
      | NUM 
      | '-' NUM {$$.c = "0" + $2.c + "-";}
      | Expr '(' ARGUMENTS ')' {$$.c = $3.c + to_string($3.counter) + $1.c + "$"; }
      | STRING 
      | LVALUE  { vector<string> vec; $$.c = vec + get_var($1.c[0], false) + "@"; } 
      | LVALUEPROP 
      | '(' Expr ')' { $$.c = $2.c; }
      | '(' '{' '}' ')' { $$.c = vector<string>{"{}"}; }
      | '{' '}' {$$.c = vector<string>{"{}"}; }
      | '[' ']' {$$.c = vector<string>{"[]"}; }
      | V_TRUE 
      | V_FALSE 
      |
      ;


%%

#include "lex.yy.c"

vector<string> declare_var( var_types type, string name, int row, int col) {
    /* cerr << "insert_symbol( " << type << ", " << name << ", " << row << ", " << col << ")" << endl; */

    auto& topo = table.back();

    if (topo.count(name) == 0) {
        topo[name] = symbol{type, row, col};
        return vector<string>{ name } + "&";
    }
    else if (type == Var && topo[name].type == Var) {
        topo[name] = symbol{type, row, col};
        return vector<string>{ };
    }
    else {
        cerr << "Erro: a variável '" << name << "' já foi declarada na linha " << topo[name].row + 1 << "." << endl;
        exit(1);
    }
}

string get_var (string &name, bool allowMod) {
  check_symbol(name, allowMod);
  string s = name;
  return s;
}

string get_field (string &name){
  string s = name;
  return s;
}

void check_symbol(string name, bool allowMod) {
    for (int i = table.size() - 1; i >= 0; i--) {
      auto& atual = table[i];
      if (atual.count(name) > 0) {
        if (allowMod && atual[name].type == Const) {
            cerr << "Variavel '" << name << "' não pode ser modificada." << endl;
            exit(1);
        }
        else {
          return;
        }
      }
    }
    cerr << "Erro: a variável '" << name << "' não foi declarada." << endl;
    exit(1);

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
