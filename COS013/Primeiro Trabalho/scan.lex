%{
    // Código C/C++
#include <iostream>
#include <string>
#include <cctype>

using namespace std;

string lexema;


std::string cleanQuotes(const std::string& str) {
    if (str.length() >= 2) {
        return str.substr(1, str.length() - 2);
    } else {
        return "";
    }
}

std::string cleanEscapedCharacters(const std::string &str) {
    std::string novaString;
    novaString.reserve(str.size());
    
    for (size_t i = 0; i < str.size(); ++i) {
        if (str[i] == '\\' && i > 0 && i < str.size() - 1) {
            switch (str[i + 1]) {
                case '\'':
                    novaString.push_back('\'');
                    ++i;
                    break;
                case '\"':
                    novaString.push_back('\"');
                    ++i;
                    break;
                default:
                    novaString.push_back(str[i]);
                    break;
            }
        } else if (i < str.size() - 1 && str[i] == str[i + 1] && ((str[i] == '\'' || str[i] == '\"') && isalpha(str[i+2]))){
            novaString.push_back(str[i]);
            ++i;
        } else {
            novaString.push_back(str[i]);
        }
    }
    
    return novaString;
}

std::string validateIdentifier(const std::string &str) {
    if (str.empty() || std::isdigit(str[0])) {
        cout << "Erro: Identificador invalido: " + str + '\n';
        return "E";
    }
    if (str[0] == '$') {
        for (size_t i = 1; i < str.size(); ++i) {
            if (!std::isalnum(str[i]) && str[i] != '_') {
                cout << "Erro: Identificador invalido: " + str + '\n';
                return "E";
            }
        }
    } else {
        for (size_t i = 0; i < str.size(); ++i) {
            if (!std::isalnum(str[i]) && str[i] != '_') {
                cout << "Erro: Identificador invalido: " + str + '\n';
                return "E";
            }
        }
    }
    return str;
}

bool verifyExpression(const std::string& str) {
    size_t posOpenBracket = str.find("${");
    if (posOpenBracket != std::string::npos) {
        size_t posCloseBracket = str.find("}", posOpenBracket);
        if (posCloseBracket != std::string::npos) {
            return true;
        }
    }
    return false;
}

void extractText(const std::string& str) {
    std::string start, expression, end;

    size_t posOpenBracket = str.find("${");
    size_t posCloseBracket = str.find("}", posOpenBracket);
    
    expression = str.substr(posOpenBracket + 2, posCloseBracket - posOpenBracket - 2);


    start = str.substr(0, posOpenBracket);
    end = str.substr(posCloseBracket + 1, str.length() - 1);

    cout << std::to_string(_STRING2) + " " + start + '\n';
    cout << std::to_string(_EXPR) + " " + expression + '\n';
    cout << std::to_string(_STRING2) + " " + end + '\n';

    return;
}

%}
 // Declarações para os patterns
WS [ \n\r\t]+
D [0-9]
L [a-zA-Z_]

INT {D}+
FLOAT {D}+(\.{D}+)?([Ee][+\-]?{D}+)?
ID ([$]|{L}|"_")*({D})*

CARACTERES_ESCAPE (\\a)|(\\b)|(\\e)|(\\f)|(\\n)|(\\r)|(\\t)|(\\y)|(\\)|(\\?)|(\\nnn)|(\\)|(\/)
ASPAS_DUPLAS_ESCAPADA (\\\")|(\"\")
ASPAS_SIMPLES_ESCAPADA (\\\')|(\'\')

ABRE_COMENTARIO (\/\*)
FECHA_COMENTARIO (\*\/)
COMENTARIO "/*"([^*]|(\*+[^*/]))*\*+"/"|(\/\/.*)

ASPAS_DUPLAS \"({L}|{D}|{WS}|\'|{ASPAS_DUPLAS_ESCAPADA}|{CARACTERES_ESCAPE}|{COMENTARIO})*\"
ASPAS_SIMPLES \'({L}|{D}|{WS}|\"|{ASPAS_SIMPLES_ESCAPADA}|{CARACTERES_ESCAPE}|{COMENTARIO})*\'


STRING {ASPAS_DUPLAS}|{ASPAS_SIMPLES}
STRING2 \`[^`]*:

ABRE_EXPRESSION a/b

IF [Ii][Ff]
FOR [Ff][Oo][Rr]
MAIG ">="
MEIG "<="
IG "=="
DIF "!="

%%
 // Patterns para yytext

{WS}            { }

{INT}           { lexema = yytext; return _INT;}

{FLOAT}         { lexema = yytext; return _FLOAT;}

{IF}            { lexema = yytext; return _IF;}

{FOR}           { lexema = yytext; return _FOR ;}

{MAIG}          { lexema = yytext; return _MAIG ;}

{MEIG}          { lexema = yytext; return _MEIG ;}

{IG}            { lexema = yytext; return _IG ;}

{DIF}           { lexema = yytext; return _DIF ;}

{COMENTARIO}    { lexema = yytext; return _COMENTARIO;}

{STRING}        { lexema = cleanEscapedCharacters(cleanQuotes(yytext)); return _STRING;}

{STRING2}       { lexema = cleanQuotes(yytext); return _STRING2;}

{ID}            { lexema = validateIdentifier(yytext); if (lexema.at(0) != 'E'){ return _ID ;};}

.               { lexema = yytext; return yytext[0];}
%%  

 // Funções auxiliares