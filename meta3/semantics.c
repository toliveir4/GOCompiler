/*META 3 - COMPILADORES 2021-2022*/
/*MARCO PAIS Nº 2019218981*/
/*TIAGO OLIVEIRA Nº 2019219068*/

#include "semantics.h"
#include <ctype.h>

struct globalTable *Head =NULL;

void criaTabelas(no_ast* atual){
    atual = atual->filho; // salta program
    no_ast* globalHead = atual; // guarda o inicio da lista de funções e variaveis globais

    // adiciona funcoes e variaveis globais
    while(strcmp(atual->tipo, "FuncDecl") == 0 || strcmp(atual->tipo, "VarDecl") == 0){
        if(strcmp(atual->tipo, "FuncDecl") == 0){
            addFunc(atual);
        }
        else if(strcmp(atual->tipo, "VarDecl") == 0){
            addGlobalVar(atual);
        }

        atual = atual->irmao; // passa para a proxima declaracao
        if(atual == NULL){
            break;
        }
    }

    atual = globalHead;

    // adiciona vars locais e anota a AST
}

void printTabelas(){
}

void addFunc(no_ast* atual){
    struct globalTable* aux = Head;

    char* funcName = (char *) malloc(sizeof(char) * strlen(atual->filho->filho->valor) + 1);
    strcpy(funcName, atual->filho->filho->valor); // FuncDecl->FuncHeader->Id(nome)

    char* funcType = (char *) malloc(sizeof(char) * strlen(atual->filho->filho->irmao->tipo) + 1);
    strcpy(funcType, atual->filho->filho->irmao->tipo); // FuncDecl->FuncHeader->Id->FuncParams

    // verifica se já existe uma função com o mesmo nome
    if (existsGlobal(funcName, atual->filho->filho, 1)){
        return;
    }  

    if(!Head){
        // primeiro elemento da lista global
        Head = (struct globalTable *) malloc(sizeof(globalTable));

        if(strcmp(funcType, "FuncParams") == 0){
            funcType = "none";
        }

        Head->func = 1;
        Head->declared = 0;
        Head->name = funcName;
        Head->type = funcType;
        Head->next = NULL;
        Head->params = NULL;

        addFuncParams(atual, Head);
        
        return;
    }

    // procura o último elemento da lista
    while(aux->next) aux = aux->next;

    aux->next = (struct globalTable *) malloc(sizeof(globalTable));
    aux = aux->next;

    if(strcmp(funcType, "FuncParams") == 0){
        funcType = "none";
    }

    aux->func = 1;
    aux->declared = 0;
    aux->name = funcName;
    aux->type = funcType;
    aux->next = NULL;
    aux->params = NULL;

    addFuncParams(atual, aux);

    return;
}

void addFuncParams(no_ast* atual, globalTable* func){
    no_ast* nodeAux=atual->filho->filho; // FuncDecl->FuncHeader->Id
    nodeAux = nodeAux->irmao;

    // procura por "FuncParams"
    if(strcmp(nodeAux->tipo, "FuncParams") != 0)
        nodeAux = nodeAux->irmao;

    // verifica se existem parâmetros
    if(!nodeAux->filho)
        return;

    nodeAux=nodeAux->filho; // FuncParams->ParamDecl

    funcParams* paramsHead = NULL; // guarda a localizacao do primeiro parametro
    funcParams* aux = NULL; // usado para verificar se já existe o parametro
    funcParams* aux1 = NULL; // usado para criar os elementos da lista ligada
    int flag; // usado para sinalizar repetição de parametros

    // percorre todos os "ParamDecl"
    while(nodeAux){
        flag = 0;
        aux = paramsHead;

        while(aux){
            // ParamDecl->Type->Id(valor)
            if(strcmp(aux->name, nodeAux->filho->irmao->valor) == 0){
                flag = 1;
                printf("Line %d, column %d: Symbol %s already defined\n",nodeAux->filho->irmao->line,nodeAux->filho->irmao->column,nodeAux->filho->irmao->valor);
            }
            aux = aux->next;
        }

        if(!flag){
            if(!aux1){
                aux1 = (struct funcParams *) malloc(sizeof(funcParams));
                
                aux1->name=(char *) malloc(sizeof(char) * strlen(nodeAux->filho->irmao->valor) + 1); 
                strcpy(aux1->name, nodeAux->filho->irmao->valor); // ParamDecl->Type->Id(valor)
                aux1->type=(char *) malloc(sizeof(char) * strlen(nodeAux->filho->tipo) + 1);
                strcpy(aux1->type, nodeAux->filho->tipo); // ParamDecl->Type
                aux1->next = NULL;

                paramsHead = aux1; // guarda o primeiro elemento da lista
            }
            else{
                aux1->next = (struct funcParams *) malloc(sizeof(funcParams));
                aux1 = aux1->next;

                aux1->name=(char *) malloc(sizeof(char) * strlen(nodeAux->filho->irmao->valor) + 1); 
                strcpy(aux1->name, nodeAux->filho->irmao->valor); // ParamDecl->Type->Id(valor)
                aux1->type=(char *) malloc(sizeof(char) * strlen(nodeAux->filho->tipo) + 1);
                strcpy(aux1->type, nodeAux->filho->tipo); // ParamDecl->Type
                aux1->next = NULL;
            }
        }

        nodeAux = nodeAux->irmao;
    }

    func->params = paramsHead;
}

void addGlobalVar(no_ast* atual){
    char* varName = (char *) malloc(sizeof(char) * strlen(atual->filho->irmao->valor) + 1);
    strcpy(varName, atual->filho->irmao->valor); // VarDecl->Type->Id(nome)

    char* varType = (char *) malloc(sizeof(char) * strlen(atual->filho->tipo) + 1);
    strcpy(varType, atual->filho->tipo); // VarDecl->Type

    // verifica se já existe uma variavel global com o mesmo nome
    if (existsGlobal(varName, atual->filho->irmao, 2)){
        return;
    }

    struct globalTable *aux = (struct globalTable *) malloc(sizeof(globalTable));

    if(!Head){
        // primeiro elemento da lista global
        Head = (struct globalTable *) malloc(sizeof(globalTable));
        
        Head->func = 0;
        Head->name = varName;
        Head->type = varType;
        Head->next = NULL;

        return;
    }

    // procura o fim da lista global
    while(aux->next) aux = aux->next;

    aux->next = (struct globalTable *) malloc(sizeof(globalTable));
    aux = aux->next;

    aux->func = 0;
    aux->name = varName;
    aux->type = varType;
    aux->next = NULL;

    return;
}

int existsGlobal(char *name, no_ast *atual, int opcao){
    // 1 - funcoes     2 - variaveis globais

    struct globalTable *aux = Head;

    while(aux){
        // percorre a lista global e compara os nomes
        if(strcmp(name, aux->name) == 0){
            if(opcao == 1){
                printf("Line %d, column %d: Symbol %s already defined\n", atual->line, atual->column, name);
                return 1;
            }
            else if(opcao == 2){
                printf("Line %d, column %d: Symbol %s already defined\n", atual->line, atual->column, name);
                return 1;
            }
        }
        aux = aux->next;
    }
    return 0;
}