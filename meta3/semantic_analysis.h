/*META 3 - COMPILADORES 2021-2022*/
/*MARCO PAIS Nº 2019218981*/
/*TIAGO OLIVEIRA Nº 2019219068*/

#ifndef SEMANTIC_ANALYSIS_H
#define SEMANTIC_ANALYSIS_H

#include "struct.h"

typedef struct func_table{ // representa uma lista ligada de funcores e de variaveis globais
    char* name;
    char* type;
    int func;
    int declared;
    struct var_table *vars; //
    struct param_table *params;
    struct func_table *next;
    
} func_table;

#endif //!SEMANTIC_ANALYSIS_H
