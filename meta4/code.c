/*META 4 - COMPILADORES 2021-2022*/
/*MARCO PAIS Nº 2019218981*/
/*TIAGO OLIVEIRA Nº 2019219068*/

#include "code.h"

extern globalTable *Head; // cabeça das tabelas de símbolos

void geraCode(no_ast *atual){
    globalTable *aux = Head;

    while(aux){
        aux = aux->next;
    }
}
