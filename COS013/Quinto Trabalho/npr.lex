%{
#include <unordered_set>
#include <sstream>
int token(int token);
string lexema;

std::string trim(const std::string& input, const std::string& charsToRemove) {
    std::unordered_set<char> removeSet(charsToRemove.begin(), charsToRemove.end());
    std::string result;
    
    for (char c : input) {
        if (removeSet.find(c) == removeSet.end()) {
            result += c;
        }
    }

    return result;
}

std::vector<std::string> tokeniza(const std::string& input) {
    std::vector<std::string> tokens;
    std::istringstream stream(input);
    std::string token;
    
    while (stream >> token) {
        tokens.push_back(token);
    }

    return tokens;
}
%}

D	[0-9]
L	[a-zA-Z_]

STRING1	\"(\"\"|\\\"|[^\"])*\"
STRING2	\'(\'\'|\\\'|[^\'])*\'
STRING {STRING1}|{STRING2}

INT	{D}+
NUM	{INT}("."{INT})?([Ee]("+"|"-")?{INT})?
ID	{L}({L}|{D})*

SEMICOLON ;+

%%

\t        { col += 4; }
" "         { col++; }
\n        { row++; col = 0;}

"+="      { return token(PLUS_EQUAL); }
"++"      { return token(INCREMENT); }
"=="      { return token(EQUALS); }
\>         { return token(GT); }
\<         { return token(ST); }

true      { return token(V_TRUE); }
false     { return token(V_FALSE); }
let       { return token(LET); }
const     { return token(CONST); }
var       { return token(VAR); }
if        { return token(IF); }
else      { return token(ELSE); }
for       { return token(FOR); }
while     { return token(WHILE); }
function  { return token(FUNCTION);}
return    { return token(RETURN); }
"asm{".*"}" { lexema = trim(yytext + 3, "{}");
              yylval.c = tokeniza(lexema);
              col += strlen( yytext);
              return ASM;
}

{STRING}    { return token(STRING); }
{NUM}   	{ return token(NUM); }

{ID}        { return token(ID); }

{SEMICOLON} { return token(DELIMITER); }

.           { return token(*yytext); }


%%

int token (int token) {
    yylval.c = vector<string>{yytext};
    col += strlen(yytext);
    yylval.row = row;
    yylval.col = col;

    return token;
}