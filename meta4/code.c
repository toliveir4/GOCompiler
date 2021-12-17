/*META 4 - COMPILADORES 2021-2022*/
/*MARCO PAIS Nº 2019218981*/
/*TIAGO OLIVEIRA Nº 2019219068*/

#include "code.h"

extern globalTable *Head; // cabeça das tabelas de símbolos
extern no_ast *root; // raiz da AST

int fvCounter = 1; // conta as variáveis de uma função

void geraCode(){
    globalTable *aux = Head;

    // declara printf e atoi
	printf("declare i32 @atoi(i8*)\n");
	printf("declare i32 @printf(i8*, ...)\n");

    // primeiro gera todas as variáveis globais
    while(aux){
        if(!aux->func){
            if(strcmp(aux->type, "int") == 0)
                printf("@%s = global i32\n", aux->name);
            else if(strcmp(aux->type, "float32") == 0)
                printf("@%s = global double\n", aux->name);
            else if(strcmp(aux->type, "bool") == 0)
                printf("@%s = global i1\n", aux->name);
            else if(strcmp(aux->type, "string") == 0)
                printf("@%s = global i8*\n", aux->name);
        }
        aux = aux->next;
    }

    aux = Head;
    // depois gera todas as funções e o seu conteúdo
    while(aux){
        if(aux->func)
            geraFunc(aux);
        aux = aux->next;
    }

    printStr();
    printAST(root, 0);
}

void geraFunc(globalTable *func){
    fvCounter = 1;
    printf("\ndefine ");

    // tipo de return
    if(strcmp(func->type, "int") == 0)
        printf("i32 ");
    else if(strcmp(func->type, "float32") == 0)
        printf("double ");
    else if(strcmp(func->type, "bool") == 0)
        printf("i1 ");
    else if(strcmp(func->type, "string") == 0)
            printf("i8* "); 
    else if(strcmp(func->type, "none") == 0)
        printf("void ");

    // nome da função
    printf("@%s(", func->name);

    // argumentos
    funcParams *pAux = func->params;
    while(pAux){
        if(strcmp(pAux->type, "int") == 0)
			printf("i32 ");
        else if(strcmp(pAux->type, "float32") == 0)
            printf("double ");
        else if(strcmp(pAux->type, "bool") == 0)
            printf("i1 ");
        else if(strcmp(pAux->type, "string") == 0)
            printf("i8* "); 
        printf("%%%s", pAux->name);
        if(pAux->next)
            printf(", ");
        pAux = pAux->next;
    }
    printf(") {\n");

    geraFuncParams(func->params);
    geraFuncLocalVars(func->vars);

    no_ast *noAux = getFuncNode(func->name, root); // "FuncDecl" da função
    if(noAux)
        noAux = noAux->filho->irmao->filho; // primeiro statement da função
    while(noAux){
        geraOperacoes(noAux, func);
            printf("\n");
        noAux = noAux->irmao;
    }

    printf("}\n");
}

void geraFuncParams(funcParams * params){
    while(params){
        printf("\t%%%d = alloca ", fvCounter);
        if(strcmp(params->type, "int") == 0)
			printf("i32\n");
        else if(strcmp(params->type, "float32") == 0)
            printf("double\n");
        else if(strcmp(params->type, "bool") == 0)
            printf("i1\n");
        else if(strcmp(params->type, "string") == 0)
            printf("i8*\n");

        printf("\tstore ");
        if(strcmp(params->type, "int") == 0)
			printf("i32 %%%s, i32* %%%d\n", params->name, fvCounter);
        else if(strcmp(params->type, "float32") == 0)
            printf("double %%%s, double* %%%d\n", params->name, fvCounter);
        else if(strcmp(params->type, "bool") == 0)
			printf("i1 %%%s, i1* %%%d\n", params->name, fvCounter);
        else if(strcmp(params->type, "string") == 0)
            printf("i8* %%%s, i1* %%%d\n", params->name, fvCounter);

        if(!params->next)
            printf("\n");
        fvCounter++;
        params = params->next;
    }
}

void geraFuncLocalVars(funcVars *vars){
    while(vars){
        printf("\t%%%s = alloca ", vars->name);
        if(strcmp(vars->type, "int") == 0)
			printf("i32\n");
        else if(strcmp(vars->type, "float32") == 0)
            printf("double\n");
        else if(strcmp(vars->type, "bool") == 0)
            printf("i1\n");
        else if(strcmp(vars->type, "string") == 0)
            printf("i8*\n");

        if(!vars->next)
            printf("\n");
        vars = vars->next;
    }
}

no_ast* getFuncNode(char *name, no_ast * atual){
    if(!atual)
        return NULL;

    if(strcmp(atual->tipo, "FuncDecl") == 0 && strcmp(name, atual->filho->filho->valor) == 0)
        return atual;
    else{
        no_ast *aux1 = getFuncNode(name, atual->filho);
        no_ast *aux2 = getFuncNode(name, atual->irmao);
        if(aux1)
            return aux1;
        if(aux2)
            return aux2;
    }
    return NULL;
}

void geraOperacoes(no_ast* atual, globalTable *func){
    if(!atual)
        return;

    if(strcmp(atual->tipo, "Return") == 0){
        geraOperacoes(atual->filho, func);

        if(strcmp(func->type, "int") == 0)
            printf("\tret i32 %%%d", fvCounter - 1);
        else if(strcmp(func->type, "float32") == 0)
            printf("\tret double %%%d", fvCounter - 1);
        else if(strcmp(func->type, "bool") == 0)
            printf("\tret i1 %%%d", fvCounter - 1);
        else if(strcmp(func->type, "string") == 0)
            printf("\tret i8* %%%d", fvCounter - 1);
        else if(strcmp(func->type, "none") == 0)
            printf("\tret void");
        return;
    }
    
    else if(strcmp(atual->tipo, "IntLit") == 0){
        printf("\t%%%d = alloca i32\n", fvCounter);
        printf("\tstore i32 %d, i32* %%%d\n", atoi(atual->valor), fvCounter);
        fvCounter++;
    }
    
    else if(strcmp(atual->tipo, "RealLit") == 0){
        printf("\t%%%d = alloca double\n", fvCounter);
        printf("\tstore double %.08f, double* %%%d\n", atof(atual->valor), fvCounter);
        fvCounter++;
    }

    else if(strcmp(atual->tipo, "StrLit") == 0){
        printf("\t%%%d = alloca i8*\n", fvCounter);
        printf("\tstore i8* %s, i8** %%%d\n", atual->valor, fvCounter);
        fvCounter++;
    }
    
    else
        geraOperacoes(atual->filho, func);
}

void printStr(){

}