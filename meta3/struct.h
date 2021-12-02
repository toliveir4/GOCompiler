/*META 2 - COMPILADORES 2021-2022*/
/*MARCO PAIS Nº 2019218981*/
/*TIAGO OLIVEIRA Nº 2019219068*/

#ifndef STRUCT_H
#define STRUCT_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>


typedef struct no_ast no_ast;
struct no_ast {
    no_ast *filho;
    no_ast *irmao;
    char* tipo;
    char* valor;
};


no_ast *novoNo(char* valor,char* tipo);
void addirmao(no_ast* no, no_ast * novoirmao);
void addfilho(no_ast *no, no_ast *filho);
void printAST(no_ast *no, int nivel);

#endif //!STRUCT_H