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
  vector<string> increment_value;
  vector<string> default_value;
  int array_field_counter = 0;

  void clear() {
    default_value.clear();
    c.clear();
    row = 0;
    col = 0;
    counter = 0;
  }
  
  void insert_increment_value(string var) {
    increment_value.push_back(var);
  }

  vector<string> consume_increment() {
    vector<string> vec;
    for (int i = 0; i < increment_value.size(); i++ ) {
      vec.push_back(increment_value[i]);
      vec.push_back(increment_value[i]);
      vec.push_back("@");
      vec.push_back("1");
      vec.push_back("+");
      vec.push_back("=");
      vec.push_back("^");
    }
    return vec;
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

vector<string> get_var(string &name, bool allowMod);
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
  for( string s : codigo ){
    if (s == "\n" || s == "^") {
      cout << s << endl;;
    }

    else if (s == ".") {
      cout << endl << s << endl;
    }
    else {
      cout << s << " ";
    }
  }
  cout << endl;  
}


void printTable() {
    // Verifica se o vetor não está vazio
    if (!table.empty()) {
        // Obtem o primeiro item do vetor
        std::map<std::string, symbol>& first_map = table[0];

        // Itera sobre o mapa e imprime as chaves e valores
        for (const auto& pair : first_map) {
            std::cout << "Key: " << pair.first;
        }
    } else {
        std::cout << "The vector is empty." << std::endl;
    }
}

%}

%token	 ID IF ELSE LET CONST VAR FOR FUNCTION ASM RETURN WHILE
%token   STRING NUM V_TRUE V_FALSE
%token    DELIMITER

%right '=' INCREMENT
%nonassoc GT ST 
%nonassoc '[' ']' '(' ')'
%left   ELSE PLUS_EQUAL EQUALS
%left '-' '+'
%left  '*' '/' '%' '.' 

%%

FIM : S { $$.c = solve_address($1.c + "." + functions); print($$.c); }

S   : CMDs
    ;

CMDs: CMDs CMD  { $$.c = $1.c + $2.c; }
    |           { $$.clear();}
    ;

CMD : CMD_VAR_DECLARATION DELIMITER
    | CMD_FUNC
    | CMD_FOR
    | CMD_WHILE
    | CMD_IF
    | '{' STACK CMDs UNSTACK '}' { $$.c = "<{" + $3.c + "}>"; }
    | '{' STACK CMDs UNSTACK '}' DELIMITER { $$.c = "<{" + $3.c + "}>"; }
    | Expr ASM DELIMITER {$$.c = $1.c + $2.c + "^"; }
    | Expr DELIMITER { $$.c = $$.c + "^"; } 
    | RETURN Expr DELIMITER { $$.c = $2.c + "'&retorno'" + "@" + "~"; }
    | EMPTY_BLOCK { $$.clear(); }
    ;

CMD_FUNC  : FUNCTION ID { declare_var(Var, $2.c[0], $2.row, $2.col); }
            '(' STACK PARAMETERS ')' '{' CMDs '}'
            {
              string lbl_function = gera_label("func_" + $2.c[0]);
              string def_function = ":" + lbl_function;

              $$.c = $2.c + "&" + $2.c + "{}" + "=" + "'&funcao'" + lbl_function
              + "[=]" + "^";
              functions = functions + def_function + $6.c + $9.c + "undefined"
              + "@" + "'&retorno'" + "@" + "~";
              table.pop_back();
            }
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
    ;

CMD_FOR : FOR '(' PRIM_E DELIMITER Expr DELIMITER Expr ')' CMD
        {
          $$.increment_value = $7.increment_value;
          string lbl_fim_for = gera_label("fim_for");
          string lbl_condicao_for = gera_label("condicao_for");
          string lbl_comando_for = gera_label("comando_for");
          string def_fim_for = ":" + lbl_fim_for;
          string def_condicao_for = ":" + lbl_condicao_for;
          string def_comando_for = ":" + lbl_comando_for;

          $$.c = $3.c + def_condicao_for + $5.c + lbl_comando_for + "?" + lbl_fim_for + "#"
          + def_comando_for + $9.c + $7.c + $$.consume_increment() + "^" + lbl_condicao_for + "#" + def_fim_for;
        }
        ;

CMD_WHILE : WHILE '(' Expr ')' CMD {
              $$.increment_value = $3.increment_value;
              string lbl_comeco_while = gera_label( "lbl_comeco_while" );
              string lbl_comeco_cmds = gera_label( "lbl_comeco_cmds" );
              string lbl_fim_while = gera_label( "lbl_fim_while" );
              string def_comeco_while = ":" + lbl_comeco_while;
              string def_comeco_cmds = ":" + lbl_comeco_cmds;
              string def_fim_while = ":" + lbl_fim_while;
              $$.c = def_comeco_while + $3.c + lbl_comeco_cmds + "?" + lbl_fim_while + "#" + def_comeco_cmds + $5.c + lbl_comeco_while + "#" + def_fim_while + $$.consume_increment();
            }
CMD_IF: IF '(' Expr ')' CMD { 
          $$.increment_value = $3.increment_value;
          string lbl_true = gera_label( "lbl_true" );
          string lbl_fim_if = gera_label( "lbl_fim_if" );
          string def_true = ":" + lbl_true;
          string def_fim = ":" + lbl_fim_if;
          $$.c = $3.c + lbl_true + "?" + lbl_fim_if + "#" + def_true + $5.c + def_fim + $$.consume_increment();
        }
      | IF '(' Expr ')' CMD ELSE CMD {
            $$.increment_value = $3.increment_value;
            string lbl_true = gera_label( "lbl_true" );
            string lbl_fim_if = gera_label( "lbl_fim_if" );
            string def_true = ":" + lbl_true;
            string def_fim = ":" + lbl_fim_if;
            $$.c = $3.c + lbl_true + "?" + $7.c + lbl_fim_if 
            + "#" + def_true + $5.c + def_fim + $$.consume_increment();
      }

CMD_VAR_DECLARATION : CMD_CONST
                    | CMD_VAR
                    | CMD_LET
                    ;

CMD_LET : LET LET_VARS {$$.c = $2.c; }
        ;

LET_VARS  : LET_VAR ',' LET_VARS {$$.c = $1.c + $3.c; }
          | LET_VAR
          ;

LET_VAR : ID '=' Expr {$$.increment_value = $$.increment_value + $3.increment_value; 
                        $$.c = declare_var(Let, $1.c[0], $1.row, $1.col) + $1.c + $3.c + "=" + "^" + $$.consume_increment(); }
        | ID '=' '[' ']' { $$.c = declare_var(Let, $1.c[0], $1.row, $1.col) + $1.c + "[]" + "=" + "^"; }
        | ID '=' '{' '}' { $$.c = declare_var(Let, $1.c[0], $1.row, $1.col) + $1.c + "{}" + "=" + "^"; }
        | ID '=' OBJ_LITERAL { $$.c = declare_var(Let, $1.c[0], $1.row, $1.col) + $1.c + "{}" + "=" + $3.c +"^"; }
        | ID '=' ARRAY_LITERAL { $$.c = declare_var(Let, $1.c[0], $1.row, $1.col) + $1.c + "[]" + "=" + $3.c +"^"; }
        | ID {$$.c = declare_var(Let, $1.c[0], $1.row, $1.col); }
        ;

CMD_CONST : CONST CONST_VARS {$$.c = $2.c; }
          ;

CONST_VARS  : CONST_VAR ',' CONST_VARS { $$.c = $1.c + $3.c; }
            | CONST_VAR
            ;

CONST_VAR : ID '=' Expr {$$.increment_value = $$.increment_value + $3.increment_value; 
                        $$.c = declare_var(Const, $1.c[0], $1.row, $1.col) + $1.c + $3.c + "=" + "^" + $$.consume_increment(); }
        | ID '=' '[' ']' { $$.c = declare_var(Const, $1.c[0], $1.row, $1.col) + $1.c + "[]" + "=" + "^"; }
        | ID '=' '{' '}' { $$.c = declare_var(Const, $1.c[0], $1.row, $1.col) + $1.c + "{}" + "=" + "^"; }
        | ID '=' OBJ_LITERAL { $$.c = declare_var(Const, $1.c[0], $1.row, $1.col) + $1.c + "{}" + "=" + $3.c +"^"; }
        | ID {$$.c = declare_var(Const, $1.c[0], $1.row, $1.col); }
        ;

CMD_VAR : VAR VAR_VARS {$$.c = $2.c; }
        ;

VAR_VARS  : VAR_VAR ',' VAR_VARS { $$.c = $1.c + $3.c; }
          | VAR_VAR
          ;

VAR_VAR : ID '=' Expr {$$.increment_value = $$.increment_value + $3.increment_value; 
                        $$.c = declare_var(Var, $1.c[0], $1.row, $1.col) + $1.c + $3.c + "=" + "^" + $$.consume_increment(); }
        | ID '=' '[' ']' { $$.c = declare_var(Var, $1.c[0], $1.row, $1.col) + $1.c + "[]" + "=" + "^"; }
        | ID '=' '{' '}' { $$.c = declare_var(Var, $1.c[0], $1.row, $1.col) + $1.c + "{}" + "=" + "^"; }
        | ID '=' OBJ_LITERAL { $$.c = declare_var(Var, $1.c[0], $1.row, $1.col) + $1.c + "{}" + "=" + $3.c +"^"; }
        | ID {$$.c = declare_var(Var, $1.c[0], $1.row, $1.col); }
        ;

STACK : { table.push_back( map<string, symbol>{}); }
      ;

UNSTACK : { table.pop_back(); }
        ;

PRIM_E: CMD_LET
        | Attr { $$.c = $1.c + "^"; }
        ;

Expr  : Expr ST Expr { $$.increment_value = $1.increment_value + $3.increment_value; $$.c = $1.c + $3.c + $2.c; }
      | Expr GT Expr { $$.increment_value = $1.increment_value + $3.increment_value; $$.c = $1.c + $3.c + $2.c; }
      | Expr EQUALS Expr { $$.increment_value = $1.increment_value + $3.increment_value; $$.c = $1.c + $3.c + "=="; }
      | Expr '+' Expr { $$.increment_value = $1.increment_value + $3.increment_value; $$.c = $1.c + $3.c + $2.c; }
      | Expr '-' Expr { $$.increment_value = $1.increment_value + $3.increment_value; $$.c = $1.c + $3.c + $2.c; }
      | Expr '*' Expr { $$.increment_value = $1.increment_value + $3.increment_value; $$.c = $1.c + $3.c + $2.c; }
      | Expr '/' Expr { $$.increment_value = $1.increment_value + $3.increment_value; $$.c = $1.c + $3.c + $2.c; }
      | Expr '%' Expr { $$.increment_value = $1.increment_value + $3.increment_value; $$.c = $1.c + $3.c + $2.c; }
      | '(' Expr ')' { $$.increment_value = $2.increment_value; $$.c = $2.c; }
      | Attr
      | LVALUE { $$.c = get_var($1.c[0], false) + "@"; }
      | LVALUE INCREMENT { $$.insert_increment_value($1.c[0]); $$.c = get_var($1.c[0], true) + "@"; }
      | LVALUEPROP { $$.c = $1.c + "[@]"; }
      | Expr '(' ARGUMENTS ')' { $$.c = $3.c + to_string($3.counter) + $1.c + "$"; }
      | '{' '}' {$$.c = vector<string>{"{}"}; }
      | '[' ']' {$$.c = vector<string>{"[]"}; }
      | Number
      | STRING
      | V_TRUE 
      | V_FALSE 
      ;

Attr  : LVALUE '=' Expr {$$.increment_value = $3.increment_value; 
                        $$.c = get_var($1.c[0], true)  + $3.c + "=" + $$.consume_increment(); }
      | LVALUEPROP '=' Expr {$$.increment_value = $3.increment_value;
                            $$.c = $1.c + $3.c + "[=]" + $$.consume_increment(); }
      | LVALUE PLUS_EQUAL Expr { $$.c = $1.c + get_var($1.c[0], true) + "@" + $3.c + "+" + "="; }
      | LVALUEPROP PLUS_EQUAL Expr { $$.c = $1.c + $1.c + "[@]" + $3.c + "+" + "[=]"; }
      ;

OBJ_LITERAL : '{' FIELD_LIST '}' { $$.c = $2.c; }
            ;

FIELD_LIST : FIELDS
           ;

FIELDS : FIELD ',' FIELDS { $$.c = $1.c + $3.c; }
       | FIELD
       ;

FIELD : ID ':' Expr { $$.c = $1.c + $3.c + "[<=]"; }
      | ID ':' OBJ_LITERAL { $$.c = $1.c + "{}" + $3.c + "[<=]"; }
      | ID ':' ARRAY_LITERAL { $$.c = $1.c + "[]" + $3.c + "[<=]"; }
      ;

ARRAY_LITERAL : '[' EXPR_ARRAY ']' { $$.c = $2.c; }
              ;

EXPR_ARRAY : EXPR_ARRAY ',' Expr { $$.array_field_counter = $1.array_field_counter + $3.array_field_counter + 1; 
                                   $$.c = to_string($1.array_field_counter) + $3.c + "[<=]" + $1.c; }
           | Expr { $$.c = to_string($$.array_field_counter) + $1.c + "[<=]"; $$.array_field_counter = 1; }
           ;


LVALUE: ID
      | '(' LVALUE ')' { $$.c = $2.c; }
      ;

LVALUEPROP  : LVALUEPROP PROPS {$$.c = $1.c + "[@]" + $2.c; }
            | LVALUE PROPS { $$.c = get_var($1.c[0], false) + "@" + $2.c; }
            | LVALUEPROP '.' LVALUE { $$.c = $1.c + "[@]" + $3.c; }
            | LVALUE '.' LVALUE { $$.c = get_var($1.c[0], false) + "@" + $3.c; }
            | '(' LVALUEPROP ')' { $$.c = $2.c; } 
            ;             

PROPS : PROPS PROP {$$.c = $1.c + "[@]" + $2.c; }
      | PROP
      ;

PROP  : '[' Expr ']' { $$.c = $2.c; }
      ;

Number  : NUM 
        | '-' NUM {$$.c = "0" + $2.c + "-"; }
        ;

EMPTY_BLOCK : '{' '}'{}

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

vector<string> get_var (string &name, bool allowMod) {
  vector<string> vec;
  check_symbol(name, allowMod);
  string s = name;
  return vec + s;
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
