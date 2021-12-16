/*META 4 - COMPILADORES 2021-2022*/
/*MARCO PAIS Nº 2019218981*/
/*TIAGO OLIVEIRA Nº 2019219068*/

#include "code.h"

extern globalTable *Head; // cabeça das tabelas de símbolos

void geraCode(no_ast *atual){
    globalTable *aux = Head;

    // declara printf e atoi
	printf("declare i32 @atoi(i8*)\n");
	printf("declare i32 @printf(i8*, ...)\n");

    // primeiro adiciona as variáveis globais
    while(aux){
        if(!aux->func){
            if(strcmp(aux->type, "int") == 0){

            }
            else if(strcmp(aux->type, "float32") == 0){

            }
            else if(strcmp(aux->type, "bool") == 0){

            }
        }

        aux = aux->next;
    }
}
