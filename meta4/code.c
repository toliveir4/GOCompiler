/*META 4 - COMPILADORES 2021-2022*/
/*MARCO PAIS Nº 2019218981*/
/*TIAGO OLIVEIRA Nº 2019219068*/

#include "code.h"

extern globalTable *Head; // cabeça das tabelas de símbolos
extern no_ast *root; // raiz da AST

int fvCounter; // conta as variáveis de uma função

void geraCode(){
    globalTable *aux = Head;

    // declara printf e atoi
	printf("declare i32 @atoi(i8*)\n");
	printf("declare i32 @printf(i8*, ...)\n");
    printf("\n@.str = private unnamed_addr constant [3 x i8] c\"%%d\\00\", align 1\n");

    // primeiro gera todas as variáveis globais
    int flag = 0;
    while(aux){
        if(!aux->func){
            if(flag == 0){
                printf("\n");
                flag = 1;
            }   
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
        printf("\n");
        aux = aux->next;
    }

    printStr();
    printAST(root, 0);
}

void geraFunc(globalTable *func){
    fvCounter = 0;
    
    geraFuncHeader(func);
    geraFuncParams(func);
    geraFuncLocalVars(func);

    no_ast *noAux = getFuncNode(func->name, root); // "FuncDecl" da função
    if(noAux)
        noAux = noAux->filho->irmao->filho; // primeiro statement da função
    while(noAux){
        if(strcmp(noAux->tipo, "VarDecl") != 0 && strcmp(noAux->tipo, "NULL") != 0){
            geraOperacoes(noAux, func);
            printf("\n");
        }
        noAux = noAux->irmao;
    }

    if(strcmp(func->name, "main") == 0)
        printf("\tret i32 0\n");
    printf("}\n");
}

void geraFuncHeader(globalTable *func){
    if(strcmp(func->name, "main") == 0){
        printf("define i32 @main(i32, i8**) {\n");
        return;
    }

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
			printf("i32");
        else if(strcmp(pAux->type, "float32") == 0)
            printf("double");
        else if(strcmp(pAux->type, "bool") == 0)
            printf("i1");
        else if(strcmp(pAux->type, "string") == 0)
            printf("i8*"); 
        if(pAux->next)
            printf(", ");
        pAux = pAux->next;
    }
    printf(") {\n");
}

void geraFuncParams(globalTable *func){
    if(strcmp(func->name, "main") == 0){
        printf("\t%%argc = alloca i32\n\tstore i32 %%0, i32* %%argc\n");
        printf("\t%%argv = alloca i8**\n\tstore i8** %%1, i8*** %%argv\n\n");
        fvCounter = 2;
        return;
    }

    funcParams *params = func->params;
    while(params){
        printf("\t%%%s = alloca ", params->name);
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
			printf("i32 %%%d, i32* %%%s\n", fvCounter, params->name);
        else if(strcmp(params->type, "float32") == 0)
            printf("double %%%d, double* %%%s\n", fvCounter, params->name);
        else if(strcmp(params->type, "bool") == 0)
			printf("i1 %%%d, i1* %%%s\n", fvCounter, params->name);
        else if(strcmp(params->type, "string") == 0)
            printf("i8* %%%d, i8** %%%s\n", fvCounter, params->name);

        if(!params->next)
            printf("\n");
        params = params->next;
        fvCounter++;
    }
}

void geraFuncLocalVars(globalTable *func){
    funcVars *vars = func->vars;
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
    
    printf("%s %s %s\n", atual->tipo, atual->valor, atual->nota);
    
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
    
    else if(strcmp(atual->tipo, "Assign") == 0){
        geraOperacoes(atual->filho->irmao, func);

        if(strcmp(atual->nota, "int") == 0)
            printf("\tstore i32 %%%d, i32* %%%s\n", fvCounter - 1, atual->filho->valor);
        else if(strcmp(atual->nota, "float32") == 0)
            printf("\tstore double %%%d, double %%%s\n", fvCounter - 1, atual->filho->valor);
        else if(strcmp(atual->nota, "bool") == 0)
            printf("\tstore i1 %%%d, i1* %%%s\n", fvCounter - 1, atual->filho->valor);
        else if(strcmp(atual->nota, "string") == 0)
            printf("\tstore i8* %%%d, i8** %%%s\n", fvCounter - 1, atual->filho->valor);

        return;
    }

    else if(strcmp(atual->tipo, "ParseArgs") == 0){
        printf("\t%%%d = load i8**, i8*** %%argv\n", fvCounter);
        fvCounter++;
        printf("\t%%%d = getelementptr inbounds i8*, i8** %%%d, i64 %d\n", fvCounter, fvCounter - 1, atoi(atual->filho->irmao->valor));
        fvCounter++;
        printf("\t%%%d = load i8*, i8** %%%d\n", fvCounter, fvCounter - 1);
        fvCounter++;
        printf("\t%%%d = call i32 @atoi(i8* %%%d)\n", fvCounter, fvCounter - 1);
        fvCounter++;
        printf("\tstore i32 %%%d, i32* %%%s\n", fvCounter, atual->filho->valor);
        fvCounter++;
        return;
    }

    else if(strcmp(atual->tipo, "Print") == 0){
        geraOperacoes(atual->filho, func);

        if(strcmp(atual->filho->nota, "int") == 0)
            printf("\t%%%d = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str, i32 0, i32 0), i32 %%%d)\n", fvCounter, fvCounter - 1);
        else if(strcmp(atual->filho->nota, "float32") == 0)
            printf("\t%%%d = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str, i32 0, i32 0), double %%%d)\n", fvCounter, fvCounter - 1);
        else if(strcmp(atual->filho->nota, "string") == 0)
            printf("\t%%%d = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str, i32 0, i32 0), i8* %%%d)\n", fvCounter, fvCounter - 1);
        else if(strcmp(atual->filho->nota, "bool") == 0)
            printf("\t%%%d = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str, i32 0, i32 0), i1 %%%d)\n", fvCounter, fvCounter - 1);
        fvCounter++;
        return;
     }

    else if(strcmp(atual->tipo, "Call") == 0){
        int first = fvCounter;
        geraOperacoes(atual->filho->irmao, func);

        if(strcmp(atual->nota, "int") == 0){
            printf("\t%%%d = call i32 @%s(", fvCounter, atual->filho->valor);
            no_ast *aux = atual->filho->irmao;
            while(aux){
                if(strcmp(aux->nota, "int") == 0)
                    printf("i32 %%%d", first);
                else if(strcmp(aux->nota, "float32") == 0)
                    printf("double %%%d", first);
                else if(strcmp(aux->nota, "string") == 0)
                    printf("i8* %%%d", first);
                else if(strcmp(aux->nota, "bool") == 0)
                    printf("i1 %%%d", first);
                if(aux->irmao)
                    printf(", ");
                first++;
                aux = aux->irmao;
            }
            printf(")\n");
        }
    }

    else if (strcmp(atual->tipo, "And") == 0 || strcmp(atual->tipo, "Or") == 0){

    }

    else if (strcmp(atual->tipo, "Add") == 0 || strcmp(atual->tipo, "Sub") == 0 || strcmp(atual->tipo, "Mul") == 0 || strcmp(atual->tipo, "Div") == 0 || strcmp(atual->tipo, "Mod") == 0){

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

    else if(strcmp(atual->tipo, "Id") == 0){
        if(strcmp(atual->nota, "int") == 0)
            printf("\t%%%d = load i32, i32* %%%s\n", fvCounter, atual->valor);
        else if(strcmp(atual->nota, "float32") == 0)
            printf("\t%%%d = load double, double* %%%s\n", fvCounter, atual->valor);
        else if(strcmp(atual->nota, "bool") == 0)
            printf("\t%%%d = load i1, i1* %%%s\n", fvCounter, atual->valor);
        else if(strcmp(atual->nota, "string") == 0)
            printf("\t%%%d = load i8*, i8** %%%s\n", fvCounter, atual->valor);

        fvCounter++;
        if(atual->irmao)
            geraOperacoes(atual->irmao, func);
        return;
    }
}

void printStr(){

}