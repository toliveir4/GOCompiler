/*META 2 - COMPILADORES 2021-2022*/
/*MARCO PAIS Nº 2019218981*/
/*TIAGO OLIVEIRA Nº 2019219068*/

%{
    #include "struct.h"
    int yylex();
    int yyparse();
    void yyerror(char* msg);

    extern int line, column; // Contadores
    int syntaxError = 0;
    extern char *straux;

    no_ast *root = NULL; // Origem arvore
    // Nos auxiliares
    no_ast *aux1 = NULL;
    no_ast *aux2 = NULL;
%}

%union {
    char *valor;
    no_ast *no;
}

/* tokens */
%token SEMICOLON COMMA 
%token ASSIGN STAR DIV MINUS PLUS EQ GE GT LE LT MOD NE NOT AND OR 
%token LBRACE LPAR LSQ RBRACE RPAR RSQ 
%token PACKAGE RETURN ELSE FOR IF 
%token VAR 
%token INT FLOAT32 BOOL STRING 
%token PRINT PARSEINT FUNC CMDARGS BLANKID

%token<valor> RESERVED REALLIT INTLIT STRLIT ID

/* regras */
%type<no> Program 
%type<no> Declarations DeclarationsAux
%type<no> VarDeclaration VarSpec VarsAndStatements
%type<no> Type 
%type<no> FuncDeclaration FuncBody FuncInvocation
%type<no> Parameters
%type<no> Statement StatementSEMICOLON
%type<no> ParseArgs
%type<no> Expr
%type<no> COMMA_ID COMMA_ID_Type COMMAExpr
%type<no> IDaux

/* precedencias */
%left COMMA
%right ASSIGN

%left OR 
%left AND
%left LT LE GT GE EQ NE
%left PLUS MINUS
%left STAR DIV MOD

%right NOT
%left LPAR RPAR LSQ RSQ

%nonassoc ELSE IF

%%
Program: PACKAGE ID SEMICOLON Declarations                      {root=novoNo("Program",NULL); $$=root; addfilho($$, $4);}
        ;

Declarations: DeclarationsAux                                   {$$=$1;}
            |   /*NULL*/                                        {$$=NULL;}
            ;


DeclarationsAux:   DeclarationsAux VarDeclaration SEMICOLON     {$$=$1; addirmao($1, $2);} /* recursividade -> adiciona irmao*/
               |   VarDeclaration SEMICOLON                     {$$=$1;}
               |   DeclarationsAux FuncDeclaration SEMICOLON    {$$=$1; addirmao($1, $2);}
               |   FuncDeclaration SEMICOLON                    {$$=$1;}
               ;

VarDeclaration: VAR VarSpec                                     {$$=$2;}
              | VAR LPAR VarSpec SEMICOLON RPAR                 {$$=$3;}
              ;
IDaux: ID                                                       {$$=novoNo("Id", $1);}
     ;

VarSpec: IDaux Type                                             {$$=novoNo("VarDecl", NULL); /* cria o primeiro no */
                                                                addfilho($$, $2); /* adiciciona o filho do primeiro no */
                                                                addirmao($2, $1); /* adiciona irmao ao filho do primeiro no */
                                                                }
       | IDaux COMMA_ID Type                                    {$$=novoNo("VarDecl", NULL); /* cria o primeiro no */
                                                                addfilho($$, $3); /* adiciciona o filho do primeiro no */
                                                                addirmao($3, $1); /* adiciona irmao ao filho do primeiro no */  
                                                                addirmao($$, $2); /* adiciona-se um irmão ao pai por causa da recursividade */
                                                                aux2 = $$->irmao; /* nó inicial para percorrer a lista de vars */
                                                                while (aux2 != NULL && strcmp(aux2->filho->tipo, "semTipo") == 0) { /* percorre todos os vars */
                                                                    strcpy(aux2->filho->tipo, $3->tipo); /* atribui o tipo a todos os vars */
                                                                    aux2 = aux2->irmao; /* passa para o próximo var */
                                                                }}
       ;

COMMA_ID: COMMA IDaux COMMA_ID                                  {$$=novoNo("VarDecl", NULL); /* cria novo nó pai */
                                                                aux1=novoNo("semTipo", NULL); /* cria nó sem tipo */
                                                                addirmao($$, $3); /* adiciona um irmao ao pai */
                                                                addfilho($$, aux1); /* adiciona um filho ao pai */
                                                                addirmao(aux1, $2); /* adiciona um irmão ao filho */
                                                                }
        | COMMA IDaux                                           {$$=novoNo("VarDecl", NULL);
                                                                aux1=novoNo("semTipo", NULL);
                                                                addfilho($$, aux1);
                                                                addirmao(aux1, $2);
                                                                }
        ;

Type: INT                                                       {$$=novoNo("Int", NULL);}
    | FLOAT32                                                   {$$=novoNo("Float32", NULL);}
    | BOOL                                                      {$$=novoNo("Bool", NULL);}
    | STRING                                                    {$$=novoNo("String", NULL);}
    ;

FuncDeclaration: FUNC IDaux LPAR Parameters RPAR Type FuncBody  {$$=novoNo("FuncDecl", NULL);
                                                                aux1 = novoNo("FuncHeader", NULL);
                                                                addfilho($$, aux1);
                                                                addirmao(aux1, $7);
                                                                addfilho(aux1, $2);
                                                                addirmao($6, $4);
                                                                addirmao($2, $6);
                                                                }
               | FUNC IDaux LPAR Parameters RPAR FuncBody       {$$=novoNo("FuncDecl", NULL);
                                                                aux1 = novoNo("FuncHeader", NULL);
                                                                addfilho($$, aux1);
                                                                addirmao(aux1, $6);
                                                                addfilho(aux1, $2);
                                                                addirmao($2, $4);
                                                                }
               | FUNC IDaux LPAR RPAR Type FuncBody             {$$=novoNo("FuncDecl", NULL);
                                                                aux1 = novoNo("FuncHeader", NULL);
                                                                addfilho($$, aux1);
                                                                addirmao(aux1, $6);
                                                                addfilho(aux1, $2);
                                                                addirmao($2, $5);
                                                                addirmao($5, novoNo("FuncParams", NULL));
                                                                }
               | FUNC IDaux LPAR RPAR FuncBody                  {$$=novoNo("FuncDecl", NULL);
                                                                aux1 = novoNo("FuncHeader", NULL);
                                                                addfilho($$, aux1);
                                                                addfilho(aux1, $2);
                                                                addirmao($2, novoNo("FuncParams", NULL));
                                                                addirmao(aux1, $5); /* funcbody e funcheader são do mesmo nível */
                                                                }
               ;

Parameters: IDaux Type COMMA_ID_Type                            {$$=novoNo("FuncParams", NULL);
                                                                aux1=novoNo("ParamDecl",NULL); 
                                                                addfilho($$, aux1);
                                                                addirmao(aux1, $3);
                                                                addfilho(aux1, $2);
                                                                addirmao($2, $1);
                                                                }
          | IDaux Type                                          {$$=novoNo("FuncParams", NULL);
                                                                aux1=novoNo("ParamDecl",NULL); 
                                                                addfilho($$, aux1);
                                                                addfilho(aux1, $2);
                                                                addirmao($2, $1);
                                                                }
          ;

COMMA_ID_Type: COMMA IDaux Type COMMA_ID_Type                   {$$=novoNo("ParamDecl",NULL); 
                                                                addfilho($$,$3);
                                                                addirmao($3,$2);
                                                                addirmao($$,$4);
                                                                }
             | COMMA IDaux Type                                 {$$=novoNo("ParamDecl",NULL); 
                                                                addfilho($$,$3);
                                                                addirmao($3,$2);
                                                                }
             ;

FuncBody: LBRACE VarsAndStatements RBRACE                       {$$=novoNo("FuncBody", NULL);
                                                                addfilho($$, $2);
                                                                }
        ;

VarsAndStatements:  VarsAndStatements VarDeclaration SEMICOLON  {$$=$1; addirmao($1, $2);}
                 |  VarsAndStatements Statement SEMICOLON       {$$=$1; addirmao($1, $2);}
                 |  VarsAndStatements SEMICOLON                 {$1=$1;}
                 | /*Epsilon = NULL*/                           {$$=novoNo("NULL", NULL);}
                 ;

Statement: IDaux ASSIGN Expr                                    {$$=novoNo("Assign", NULL);
                                                                addfilho($$, $1);
                                                                addirmao($1, $3);
                                                                }
         | LBRACE RBRACE                                        {$$=novoNo("NULL", NULL);}
         | LBRACE StatementSEMICOLON RBRACE                     {aux2=$2;
                                                                int n = 0;
                                                                while(aux2 != NULL) {
                                                                    if (strcmp(aux2->tipo, "NULL") != 0) n++;
                                                                    aux2=aux2->irmao;
                                                                    }
                                                                if (n >= 2) {
                                                                    $$=novoNo("Block", NULL);
                                                                    addfilho($$, $2);
                                                                } else $$=$2;
                                                                }
         | IF Expr LBRACE RBRACE                                {$$=novoNo("If", NULL);
                                                                addfilho($$, $2);
                                                                aux1=novoNo("Block", NULL);
                                                                addirmao($2, aux1);
                                                                addirmao(aux1, novoNo("Block", NULL));
                                                                }
         | IF Expr LBRACE StatementSEMICOLON RBRACE             {$$=novoNo("If", NULL);
                                                                addfilho($$, $2);
                                                                aux1=novoNo("Block", NULL);
                                                                addirmao($2, aux1);
                                                                addfilho(aux1, $4);
                                                                addirmao(aux1, novoNo("Block", NULL));
                                                                }
         | IF Expr LBRACE RBRACE ELSE LBRACE RBRACE             {$$=novoNo("If", NULL);
                                                                addfilho($$, $2);
                                                                aux1=novoNo("Block", NULL);
                                                                addirmao($2, aux1);
                                                                addirmao(aux1, novoNo("Block", NULL));
                                                                }
         | IF Expr LBRACE StatementSEMICOLON RBRACE ELSE LBRACE RBRACE {$$=novoNo("If", NULL);
                                                                        addfilho($$, $2);
                                                                        aux1=novoNo("Block", NULL);
                                                                        addirmao($2, aux1);
                                                                        addfilho(aux1, $4);
                                                                        addirmao(aux1, novoNo("Block", NULL));
                                                                        }
         | IF Expr LBRACE RBRACE ELSE LBRACE StatementSEMICOLON RBRACE {$$=novoNo("If", NULL);
                                                                        addfilho($$, $2);
                                                                        aux1=novoNo("Block", NULL);
                                                                        aux2=novoNo("Block", NULL);
                                                                        addirmao($2, aux1);
                                                                        addirmao(aux1, aux2);
                                                                        addfilho(aux2, $7);
                                                                        }
         | IF Expr LBRACE StatementSEMICOLON RBRACE ELSE LBRACE StatementSEMICOLON RBRACE {$$=novoNo("If", NULL);
                                                                                            addfilho($$, $2);
                                                                                            aux1=novoNo("Block", NULL);
                                                                                            aux2=novoNo("Block", NULL);
                                                                                            addirmao($2, aux1);
                                                                                            addfilho(aux1, $4);
                                                                                            addirmao(aux1, aux2);
                                                                                            addfilho(aux2, $8);
                                                                                            }
         | FOR LBRACE RBRACE                                    {$$=novoNo("For", NULL);
                                                                addfilho($$, novoNo("Block", NULL));
                                                                }
         | FOR LBRACE StatementSEMICOLON RBRACE                 {$$=novoNo("For", NULL);
                                                                aux1=novoNo("Block", NULL);
                                                                addfilho($$, aux1);
                                                                addfilho(aux1, $3);
                                                                }
         | FOR Expr LBRACE RBRACE                               {$$=novoNo("For", NULL);
                                                                addfilho($$, $2);
                                                                addirmao($2, novoNo("Block", NULL));
                                                                }
         | FOR Expr LBRACE StatementSEMICOLON RBRACE            {$$=novoNo("For", NULL);
                                                                addfilho($$, $2);
                                                                aux1=novoNo("Block", NULL);
                                                                addirmao($2, aux1);
                                                                addfilho(aux1, $4);
                                                                }
         | RETURN                                               {$$=novoNo("Return", NULL);}
         | RETURN Expr                                          {$$=novoNo("Return", NULL);
                                                                addfilho($$, $2);
                                                                }
         | FuncInvocation                                       {$$=novoNo("Call", NULL);
                                                                addfilho($$, $1);
                                                                }
         | ParseArgs                                            {$$=$1;}
         | PRINT LPAR Expr RPAR                                 {$$=novoNo("Print", NULL);
                                                                addfilho($$, $3);}
         | PRINT LPAR STRLIT RPAR                               {$$=novoNo("Print", NULL);
                                                                aux1=novoNo("StrLit", $3);
                                                                addfilho($$, aux1);
                                                                }
         | error                                                {$$=novoNo("Error", NULL);}
         ;

StatementSEMICOLON: StatementSEMICOLON Statement SEMICOLON      {$$=$1; addirmao($1, $2);}
                  | Statement SEMICOLON                         {$$=$1;}
                  ;

ParseArgs: IDaux COMMA BLANKID ASSIGN PARSEINT LPAR CMDARGS LSQ Expr RSQ RPAR  {$$=novoNo("ParseArgs", NULL);
                                                                                addfilho($$, $1);
                                                                                addirmao($1, $9);
                                                                                }
         | IDaux COMMA BLANKID ASSIGN PARSEINT LPAR error RPAR  {$$=novoNo("ParseArgs", NULL);
                                                                addfilho($$, $1);
                                                                addirmao($1, novoNo("Error", NULL));
                                                                }
         ;

FuncInvocation: IDaux LPAR RPAR                                 {$$=$1;}
              | IDaux LPAR Expr RPAR                            {$$=$1; addirmao($1, $3);}
              | IDaux LPAR Expr COMMAExpr RPAR                  {$$=$1; addirmao($1, $3); addirmao($3, $4);}
              | IDaux LPAR error RPAR                           {$$=$1; addirmao($1, novoNo("Error", NULL));}
              ;

COMMAExpr: COMMAExpr COMMA Expr                                 {$$=$1; addirmao($1, $3);}
         | COMMA Expr                                           {$$=$2;}
         ;

Expr: Expr OR Expr                                              {$$=novoNo("Or", NULL);
                                                                addfilho($$, $1);
                                                                addirmao($1, $3);
                                                                }
    | Expr AND Expr                                             {$$=novoNo("And", NULL);
                                                                addfilho($$, $1);
                                                                addirmao($1, $3);
                                                                }
    | Expr LT Expr                                              {$$=novoNo("Lt", NULL);
                                                                addfilho($$, $1);
                                                                addirmao($1, $3);
                                                                }
    | Expr GT Expr                                              {$$=novoNo("Gt", NULL);
                                                                addfilho($$, $1);
                                                                addirmao($1, $3);
                                                                }
    | Expr EQ Expr                                              {$$=novoNo("Eq", NULL);
                                                                addfilho($$, $1);
                                                                addirmao($1, $3);
                                                                }
    | Expr NE Expr                                              {$$=novoNo("Ne", NULL);
                                                                addfilho($$, $1);
                                                                addirmao($1, $3);
                                                                }
    | Expr LE Expr                                              {$$=novoNo("Le", NULL);
                                                                addfilho($$, $1);
                                                                addirmao($1, $3);
                                                                }
    | Expr GE Expr                                              {$$=novoNo("Ge", NULL);
                                                                addfilho($$, $1);
                                                                addirmao($1, $3);
                                                                }
    | Expr PLUS Expr                                            {$$=novoNo("Add", NULL);
                                                                addfilho($$, $1);
                                                                addirmao($1, $3);
                                                                }
    | Expr MINUS Expr                                           {$$=novoNo("Sub", NULL);
                                                                addfilho($$, $1);
                                                                addirmao($1, $3);
                                                                }
    | Expr STAR Expr                                            {$$=novoNo("Mul", NULL);
                                                                addfilho($$, $1);
                                                                addirmao($1, $3);
                                                                }
    | Expr DIV Expr                                             {$$=novoNo("Div", NULL);
                                                                addfilho($$, $1);
                                                                addirmao($1, $3);
                                                                }
    | Expr MOD Expr                                             {$$=novoNo("Mod", NULL);
                                                                addfilho($$, $1);
                                                                addirmao($1, $3);
                                                                }
    | NOT Expr                                                  {$$=novoNo("Not", NULL);
                                                                addfilho($$, $2);
                                                                }
    | MINUS Expr        %prec DIV                               {$$=novoNo("Minus", NULL);
                                                                addfilho($$, $2);
                                                                }
    | PLUS Expr         %prec DIV                               {$$=novoNo("Plus", NULL);
                                                                addfilho($$, $2);
                                                                }
    | INTLIT                                                    {$$=novoNo("IntLit", $1);}
    | REALLIT                                                   {$$=novoNo("RealLit", $1);}
    | IDaux                                                     {$$=$1;}
    | FuncInvocation                                            {$$=novoNo("Call", NULL);
                                                                addfilho($$, $1);
                                                                }
    | LPAR Expr RPAR                                            {$$=$2;}
    | LPAR error RPAR                                           {$$=novoNo("Error", NULL);}
    ;

%%

void yyerror(char *msg) {
    // Flag de error passa a true
    syntaxError = 1;
    int i=0;
    // Conta o tamanho da string para descontar no counter
    while(straux[i]!='\0') i++;

    // Se estiver no fim da linha
    if(strcmp(yylval.valor,"\n")==0) printf("Line %d, column %d: %s: %s\n", line, column-i, msg, straux);
    
    else{
        // Se estiver no fim do ficheiro
        if(strcmp(yylval.valor,"EOF")==0){
            printf("Line %d, column %d: %s: %s\n", line, column, msg, straux);
            return;
        }
        else{
            // Conta o tamanho da string para descontar no counter
            i=0;
            while(yylval.valor[i]!='\0') i++;

            printf("Line %d, column %d: %s: %s\n", line, column-i, msg, yylval.valor);
        }
    }
}