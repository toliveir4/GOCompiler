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
    struct globalTable *next;
} globalTable;

void criaTabelas(no_ast* atual);
void printTabelas();
void addFunc(no_ast* atual);
void addGlobalVar(no_ast* atual);
int existsGlobal(char* name, no_ast* atual, int opcao);

#endif //!SEMANTICS_H
