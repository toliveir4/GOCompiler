/*META 3 - COMPILADORES 2021-2022*/
/*MARCO PAIS Nº 2019218981*/
/*TIAGO OLIVEIRA Nº 2019219068*/

#include "semantics.h"
#include <ctype.h>

globalTable *Head =NULL;

void criaTabelas(no_ast* atual){
    atual = atual->filho; // salta "Program"
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
    while(strcmp(atual->tipo, "FuncDecl") == 0 || strcmp(atual->tipo, "VarDecl") == 0){
        // se encontrar uma função
        if(strcmp(atual->tipo, "FuncDecl") == 0)
            addFuncLocal(atual);

        atual = atual->irmao; // passa para a proxima declaracao
        if(atual == NULL){
            break;
        }
    }
}

void printTabelas(){
    printf("===== Global Symbol Table =====\n");
    globalTable *aux = Head;
    while(aux){
        printf("%s", aux->name);
        if(aux->func){
            printf("\t(");
            funcParams *paramsAux = aux->params;
            if(paramsAux){
                printf("%s",paramsAux->type);
                paramsAux=paramsAux->next;
                while(paramsAux){
                    printf(",%s",paramsAux->type);
                    paramsAux=paramsAux->next;
                }
            }

            printf(")");
        }
        else{
            printf("\t");
        }
        printf("\t%s\n", aux->type);
        aux = aux->next;
    }
    
    aux = Head;
    while(aux){
        if(aux->func){
            printf("\n===== Function ");
            printf("%s(", aux->name);
            funcParams *paramsAux = aux->params;
            if(paramsAux){
                printf("%s", paramsAux->type);
                paramsAux = paramsAux->next;
                while(paramsAux){
                    printf(",%s", paramsAux->type);
                    paramsAux = paramsAux->next;
                }
            }
            printf(") Symbol Table =====\n");
            printf("%s\t\t%s\n", "return", aux->type);         // print do return 

            paramsAux = aux->params;
            while(paramsAux){
                printf("%s\t\t%s\tparam\n",paramsAux->name,paramsAux->type); // print dos parametros como variaveis locais
                paramsAux=paramsAux->next;
            }

            funcVars *varsAux = aux->vars;  // print das variaveis locais
            while(varsAux){
                printf("%s\t\t%s\n", varsAux->name, varsAux->type);
                varsAux = varsAux->next;
            }
        }
        
        aux = aux->next;
    }
    printf("\n");
}

void addFunc(no_ast* atual){
    globalTable* aux = Head;

    char* funcName = (char *) malloc(sizeof(char) * strlen(atual->filho->filho->valor) + 1);
    strcpy(funcName, atual->filho->filho->valor); // FuncDecl->FuncHeader->Id(nome)

    char* funcType = (char *) malloc(sizeof(char) * strlen(atual->filho->filho->irmao->tipo) + 1);
    strcpy(funcType, atual->filho->filho->irmao->tipo); // FuncDecl->FuncHeader->Id->FuncParams
    funcType[0] = tolower(funcType[0]);

    // verifica se já existe uma função com o mesmo nome
    if (existsGlobal(funcName, atual->filho->filho)){
        return;
    }  

    if(!Head){
        // primeiro elemento da lista global
        Head = (globalTable *) malloc(sizeof(globalTable));

        if(strcmp(funcType, "FuncParams") == 0 || strcmp(funcType,"funcParams") == 0){
            funcType = "none";
        }

        Head->func = 1;
        Head->declared = 0;
        Head->name = funcName;
        Head->type = funcType;
        Head->next = NULL;
        Head->params = NULL;
        Head->vars = NULL;

        addFuncParams(atual, Head);
        
        return;
    }

    // procura o último elemento da lista
    while(aux->next) aux = aux->next;

    aux->next = (globalTable *) malloc(sizeof(globalTable));
    aux = aux->next;


    if(strcmp(funcType, "FuncParams") == 0 || strcmp(funcType,"funcParams") == 0){
        funcType = "none";
    }

    aux->func = 1;
    aux->declared = 0;
    aux->name = funcName;
    aux->type = funcType;
    aux->next = NULL;
    aux->params = NULL;
    aux->vars = NULL;

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
                printf("Line %d, column %d: Symbol %s already defined\n", nodeAux->filho->irmao->line, nodeAux->filho->irmao->column, nodeAux->filho->irmao->valor);
            }
            aux = aux->next;
        }

        if(!flag){
            if(!aux1){
                aux1 = (funcParams *) malloc(sizeof(funcParams));
                
                aux1->name = (char *) malloc(sizeof(char) * strlen(nodeAux->filho->irmao->valor) + 1); 
                strcpy(aux1->name, nodeAux->filho->irmao->valor); // ParamDecl->Type->Id(valor)
                aux1->type = (char *) malloc(sizeof(char) * strlen(nodeAux->filho->tipo) + 1);
                strcpy(aux1->type, nodeAux->filho->tipo); // ParamDecl->Type
                aux1->type[0] = tolower(aux1->type[0]);
                aux1->next = NULL;

                paramsHead = aux1; // guarda o primeiro elemento da lista
            }
            else{
                aux1->next = (funcParams *) malloc(sizeof(funcParams));
                aux1 = aux1->next;

                aux1->name=(char *) malloc(sizeof(char) * strlen(nodeAux->filho->irmao->valor) + 1); 
                strcpy(aux1->name, nodeAux->filho->irmao->valor); // ParamDecl->Type->Id(valor)
                aux1->type=(char *) malloc(sizeof(char) * strlen(nodeAux->filho->tipo) + 1);
                strcpy(aux1->type, nodeAux->filho->tipo); // ParamDecl->Type
                aux1->type[0] = tolower(aux1->type[0]);
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
    varType[0] = tolower(varType[0]);

    // verifica se já existe uma variavel global com o mesmo nome
    if (existsGlobal(varName, atual->filho->irmao)){
        return;
    }

    globalTable *aux = (globalTable *) malloc(sizeof(globalTable));

    if(!Head){
        // primeiro elemento da lista global
        Head = (globalTable *) malloc(sizeof(globalTable));
        
        Head->func = 0;
        Head->name = varName;
        Head->type = varType;
        Head->next = NULL;

        return;
    }

    aux = Head;

    // procura o fim da lista global
    while(aux->next){
        aux = aux->next;
    }

    aux->next = (globalTable *) malloc(sizeof(globalTable));
    aux = aux->next;

    aux->func = 0;
    aux->name = varName;
    aux->type = varType;
    aux->next = NULL;

    return;
}

int existsGlobal(char *name, no_ast *atual){
    globalTable *aux = Head;

    while(aux){
        // percorre a lista global e compara os nomes
        if(strcmp(name, aux->name) == 0){
            printf("Line %d, column %d: Symbol %s already defined\n", atual->line, atual->column, name);
            return 1;
        }

        aux = aux->next;
    }
    return 0;
}

void addFuncLocal(no_ast* atual){
    no_ast* nodeAux = atual->filho->irmao->filho; // FuncDecl->FuncHeader->FuncBody->VarDecl, Return ...
    globalTable *funcP = getFuncPointer(atual->filho->filho->valor); // recebe o ponteiro da funçao na lista global

    // verifica se existe a funcao na lista global e ainda nao foi declarada
    if(funcP){
        // percorre 
        while(nodeAux){
            if(strcmp(nodeAux->tipo, "VarDecl") == 0){
                // se for uma declaracao de uma variavel
                addFuncLocalVar(nodeAux, funcP);
            }
            else if(strcmp(nodeAux->tipo, "NULL") != 0){
                // se for outra coisa como um assign, return ...
                //anotaStatementsExpressoes(nodeAux, funcP);
            }

            nodeAux=nodeAux->irmao;
        }
    }
}

globalTable* getFuncPointer(char* name){
    globalTable *aux = Head;

    while(aux){
        if(strcmp(aux->name, name) == 0 && aux->func){
            // verifica se a função já foi declarada
            if(!aux->declared){
                aux->declared = 1;
                return aux;
            }

            return NULL;
        }

        aux = aux->next;
    }

    return NULL;
}

void addFuncLocalVar(no_ast *atual, globalTable *func){
    no_ast *aux = atual->filho->irmao; // VarDecl->Type->Id
    funcParams *paramsAux = func->params;
    
    // verifica se a variavel existe nos parametros da funcao
    while(paramsAux){
        if(strcmp(paramsAux->name, aux->valor) == 0){
            printf("Line %d, column %d: Symbol %s already defined\n", aux->line, aux->column, aux->valor);
            return;
        }

        paramsAux = paramsAux->next;
    }

    funcVars *varsAux = func->vars;

    if(!varsAux){
        // primeiro elemento da lista ligada
        varsAux = (funcVars*) malloc(sizeof(funcVars));

        varsAux->name = (char*) malloc(sizeof(char) * strlen(aux->valor) + 1);
        strcpy(varsAux->name, aux->valor);
        varsAux->type = (char*) malloc(sizeof(char) * strlen(atual->filho->tipo) + 1);
        strcpy(varsAux->type, atual->filho->tipo);
        varsAux->type[0] = tolower(varsAux->type[0]);
        varsAux->next = NULL;

        func->vars = varsAux;

        // verifica se a variavel e utilizada
        if (checkLocalVarUsed(atual->irmao, varsAux->name) == 0){
            printf("Line %d, column %d: Symbol %s declared but never used\n", aux->line, aux->column, varsAux->name);
        }

        return;
    }

    // verifica se ja existe a variavel local ao mesmo tempo que percorre a lista até ao fim
    while(varsAux){
        if(strcmp(varsAux->name, aux->valor) == 0){
            printf("Line %d, column %d: Symbol %s already defined\n", aux->line, aux->column, aux->valor);
            return;
        }

        if(!varsAux->next)
            break;

        varsAux = varsAux->next;
    }

    varsAux->next = (funcVars*) malloc(sizeof(funcVars));
    varsAux = varsAux->next;

    varsAux->name = (char*) malloc(sizeof(char) * strlen(aux->valor) + 1);
    strcpy(varsAux->name, aux->valor);
    varsAux->type = (char*) malloc(sizeof(char) * strlen(atual->filho->tipo) + 1);
    strcpy(varsAux->type, atual->filho->tipo);
    varsAux->type[0] = tolower(varsAux->type[0]);
    varsAux->next = NULL;

    // verifica se a variavel e utilizada
    if (checkLocalVarUsed(atual->irmao, varsAux->name) == 0){
        printf("Line %d, column %d: Symbol %s declared but never used\n", aux->line, aux->column, varsAux->name);
    }
}

int checkLocalVarUsed(no_ast *atual, char *name){
    // se chegar ao fim
    if(!atual)
        return 0;

    // procura a variavel em tudo menos em "VarDecl" porque não tem sentido
    if(strcmp(atual->tipo, "VarDecl") == 0)
        return checkLocalVarUsed(atual->irmao, name);

    // enquanto não encontra um "Id" percorre a estrutura
    if(strcmp(atual->tipo, "Id") != 0)
        return checkLocalVarUsed(atual->filho, name) || checkLocalVarUsed(atual->irmao, name);

    // quando encontrar um "Id"
    // se nao for igual ao nome da variavel continua a procurar
    if(strcmp(atual->valor, name) != 0)
        return checkLocalVarUsed(atual->filho, name) || checkLocalVarUsed(atual->irmao, name);

    // se encontrar um "Id" e um nome correspondente retorna 1
    return 1;
}