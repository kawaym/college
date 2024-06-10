%{
int token(int token);
%}

D	[0-9]
L	[a-zA-Z_]

STRING1	\"(\"\"|\\\"|[^\"])*\"
STRING2	\'(\'\'|\\\'|[^\'])*\'
STRING {STRING1}|{STRING2}

INT	{D}+
NUM	[+|-]?{INT}("."{INT})?([Ee]("+"|"-")?{INT})?
ID	{L}({L}|{D})*
PRINT	[Pp][Rr][Ii][Nn][Tt]

%%

\t        { col += 4; }
" "         { col++; }
\n        { row++; col = 0;}

"+="      { return token(PLUS_EQUAL); }
"++"      { return token(DOUBLE_PLUS); }
"=="      { return token(DOUBLE_EQUAL); }
\>         { return token(GT); }
\<         { return token(ST); }

let       { return token(LET); }
const     { return token(CONST); }
var       { return token(VAR); }
if        { return token(IF); }
else      { return token(ELSE); }
{PRINT}     { return token(PRINT); }
for       { return token(FOR); }

{STRING}    { return token(STRING); }
{NUM}   	{ return token(NUM); }

{ID}        { return token(ID); }

.           { return token(*yytext); }


%%

int token (int token) {
    yylval.c = vector<string>{yytext};
    col += strlen(yytext);
    yylval.row = row;
    yylval.col = col;

    return token;
}