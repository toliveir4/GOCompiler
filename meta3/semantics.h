/*META 3 - COMPILADORES 2021-2022*/
/*MARCO PAIS Nº 2019218981*/
/*TIAGO OLIVEIRA Nº 2019219068*/

#ifndef SEMANTICS_H
#define SEMANTICS_H

#include "struct.h"

typedef struct globalTable{ // lista ligada de funcoes e variaveis globais
    char* name;
    char* type;
    int func;
    int declared;
    struct funcParams* params;
    struct globalTable *next;
} globalTable;

typedef struct funcParams{ // representa uma lista ligada de parametros de uma funcao
    char* name;
    char* type;
    struct funcParams *next;
} funcParams;

void criaTabelas(no_ast* atual);
void printTabelas();
void addFunc(no_ast* atual);
void addFuncParams(no_ast* atual, globalTable* func);
void addGlobalVar(no_ast* atual);
int existsGlobal(char* name, no_ast* atual, int opcao);

#endif //!SEMANTICS_H
