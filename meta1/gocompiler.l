/*META 1 - COMPILADORES 2021-2022*/
/*MARCO PAIS Nº 2019218981*/
/*TIAGO OLIVEIRA Nº 2019219068*/

%{
    #include <string.h>
    int line = 1, column = 1, line_com, col_com, line_str, col_str;
    int flag = 0, flag_escape = 0;
    char aux_str[512];
%}

NEWLINE             \n|\r|\r\n
ESCAPE              \\f|\\n|\\r|\\t|\\\\|\\\"
NUMBER              [0-9]
OCTAL               0[0-7]*
ILLEGAL_OCTAL       0{NUMBER}*
HEX                 0[xX][0-9A-Fa-f]+
DECIMAL             [1-9]{NUMBER}*
EXPONENT            [eE][+-]?{NUMBER}+
LETTER              [a-zA-Z_]
SPACE               " "
TAB                 \t

ID                  {LETTER}({LETTER}|{NUMBER})* 
INTLIT              ({OCTAL}|{HEX}|{DECIMAL})
REALLIT             ({NUMBER}+"."{NUMBER}*{EXPONENT}?)|("."{NUMBER}+{EXPONENT}?)|{NUMBER}+{EXPONENT}
SEMICOLON           ";"
COMMA               ","
BLANKID             "_"
ASSIGN              "="
STAR                "*"
DIV                 "/"
MINUS               "-"
PLUS                "+"
EQ                  "=="
GE                  ">="
GT                  ">"
LE                  "<="
LT                  "<"
MOD                 "%"
NE                  "!="
NOT                 "!"
AND                 "&&"
OR                  "||"
LBRACE              "{"
LPAR                "("
LSQ                 "["
RBRACE              "}"
RPAR                ")"
RSQ                 "]"
PACKAGE             package
RETURN              return
ELSE                else
FOR                 for
IF                  if
VAR                 var
INT                 int
FLOAT32             float32
BOOL                bool
STRING              string
PRINT               fmt.Println
PARSEINT            strconv.Atoi
FUNC                func
CMDARGS             os.Args
RESERVED            break|default|interface|select|case|defer|go|map|struct|chan|goto|switch|const|fallthrough|range|type|continue|import|{PLUS}{2}|{MINUS}{2}

%X LINE_COMMENT GENERAL_COMMENT STRLIT
%%

{SEMICOLON}         {printf("SEMICOLON\n"); flag = 0; column += yyleng;}
{COMMA}             {printf("COMMA\n"); flag = 0; column += yyleng;}
{ASSIGN}            {printf("ASSIGN\n"); flag = 0; column += yyleng;}
{STAR}              {printf("STAR\n"); flag = 0; column += yyleng;}
{DIV}               {printf("DIV\n"); flag = 0; column += yyleng;}
{MINUS}             {printf("MINUS\n"); flag = 0; column += yyleng;}
{PLUS}              {printf("PLUS\n"); flag = 0; column += yyleng;}
{EQ}                {printf("EQ\n"); flag = 0; column += yyleng;}
{GE}                {printf("GE\n"); flag = 0; column += yyleng;}
{GT}                {printf("GT\n"); flag = 0; column += yyleng;}
{LE}                {printf("LE\n"); flag = 0; column += yyleng;}
{LT}                {printf("LT\n"); flag = 0; column += yyleng;}
{MOD}               {printf("MOD\n"); flag = 0; column += yyleng;}
{NE}                {printf("NE\n"); flag = 0; column += yyleng;}
{NOT}               {printf("NOT\n"); flag = 0; column += yyleng;}
{AND}               {printf("AND\n"); flag = 0; column += yyleng;}
{OR}                {printf("OR\n"); flag = 0; column += yyleng;}
{LBRACE}            {printf("LBRACE\n"); flag = 0; column += yyleng;}
{LPAR}              {printf("LPAR\n"); flag = 0; column += yyleng;}
{LSQ}               {printf("LSQ\n"); flag = 0; column += yyleng;}
{RBRACE}            {printf("RBRACE\n"); flag = 1; column += yyleng;}
{RPAR}              {printf("RPAR\n"); flag = 1; column += yyleng;}
{RSQ}               {printf("RSQ\n"); flag = 1; column += yyleng;}
{PACKAGE}           {printf("PACKAGE\n"); flag = 0; column += yyleng;}
{RETURN}            {printf("RETURN\n"); flag = 1; column += yyleng;}
{ELSE}              {printf("ELSE\n"); flag = 0; column += yyleng;}
{FOR}               {printf("FOR\n"); flag = 0; column += yyleng;}
{IF}                {printf("IF\n"); flag = 0; column += yyleng;}
{VAR}               {printf("VAR\n"); flag = 0; column += yyleng;}
{INT}               {printf("INT\n"); flag = 0; column += yyleng;}
{FLOAT32}           {printf("FLOAT32\n"); flag = 0; column += yyleng;}
{BOOL}              {printf("BOOL\n"); flag = 0; column += yyleng;}
{STRING}            {printf("STRING\n"); flag = 0; column += yyleng;}
{PRINT}             {printf("PRINT\n"); flag = 0; column += yyleng;}
{PARSEINT}          {printf("PARSEINT\n"); flag = 0; column += yyleng;}
{FUNC}              {printf("FUNC\n"); flag = 0; column += yyleng;}
{CMDARGS}           {printf("CMDARGS\n"); flag = 0; column += yyleng;}
{RESERVED}          {printf("RESERVED(%s)\n", yytext); flag = 0; column += yyleng;}
{BLANKID}           {printf("BLANKID\n"); flag = 0; column += yyleng;}

{REALLIT}           {printf("REALLIT(%s)\n", yytext); flag = 1; column += yyleng;}
{INTLIT}            {printf("INTLIT(%s)\n", yytext); flag = 1; column += yyleng;}
{ILLEGAL_OCTAL}     {printf("Line %d, column %d: invalid octal constant (%s)\n", line, column, yytext); flag = 0; column += yyleng;}
{ID}                {printf("ID(%s)\n", yytext); flag = 1; column += yyleng;}

\"                              {line_str = line; col_str = column; 
                                strcat(aux_str, yytext); 
                                column += yyleng; 
                                flag_escape = 0;
                                BEGIN STRLIT;}

<STRLIT>{NEWLINE}               {printf("Line %d, column %d: unterminated string literal\n", line_str, col_str); 
                                memset(&aux_str, 0, sizeof(aux_str)); 
                                line++; column = 1;
                                flag = 0;
                                BEGIN 0;}

<STRLIT>{ESCAPE}                {column += yyleng; strcat(aux_str, yytext);}

<STRLIT>(\\.|\\)                {printf("Line %d, column %d: invalid escape sequence (%s)\n", line, column, yytext); 
                                column += yyleng; flag_escape = 1; flag = 0; 
                                memset(&aux_str, 0, sizeof(aux_str));}

<STRLIT>"\""                    {strcat(aux_str, yytext); 
                                if(flag_escape == 0) {printf("STRLIT(%s)\n", aux_str); flag = 1;}
                                memset(&aux_str, 0, sizeof(aux_str)); 
                                BEGIN 0;
                                column += yyleng;}

<STRLIT>.                       {column += yyleng; 
                                strcat(aux_str, yytext);}

"//"                            {BEGIN LINE_COMMENT;}
<LINE_COMMENT>{NEWLINE}         {BEGIN 0; line++; column = 1;}
<LINE_COMMENT>.                 {column += yyleng;}

"/*"                            {line_com = line; col_com = column; BEGIN GENERAL_COMMENT; column += yyleng;}
<GENERAL_COMMENT>{NEWLINE}      {line++; column = 1;}
<GENERAL_COMMENT><<EOF>>        {printf("Line %d, column %d: unterminated comment\n", line_com, col_com); yyterminate();}
<GENERAL_COMMENT>.              {column += yyleng;}
<GENERAL_COMMENT>"*/"           {BEGIN 0; column += yyleng;}

{SPACE}     {column += yyleng;}
{TAB}       {column += yyleng;}
{NEWLINE}   {if(flag == 1) printf("SEMICOLON\n"); flag = 0; line++; column = 1;}
<<EOF>>     {if(flag == 1) printf("SEMICOLON\n"); flag = 0; yyterminate();}
.           {flag = 0; printf("Line %d, column %d: illegal character (%s)\n", line, column, yytext); column += yyleng;}

%%

int main()
{
yylex();
return 0;
}

int yywrap()
{
return 1;
}
