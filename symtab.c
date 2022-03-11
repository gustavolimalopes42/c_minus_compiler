#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"


static int hash ( char * key ){
  int temp = 0;
  int i = 0;
  while (key[i] != '\0'){ 
    temp = ((temp << SHIFT) + key[i]) % SIZE;
    ++i;
  }
  return temp;
}

void st_insert( char * name, int lineno, int memloc, char* escopo, dataTypes RetType, dataTypes StmtType, IDTypes IType, int vet ){
  int h = hash(name);
  BucketList l =  hashTable[h];

  if(IType == CALL){
    // Procura a ultima declaração com o mesmo nome
    while ((l != NULL)){
      if(((strcmp(name,l->name) == 0))){
        if(l->IType == CALL){
          break;
        }
      }
      l = l->next;
    }
    if(l == NULL){
      l = (BucketList) malloc(sizeof(struct BucketListRec));
      l->name = name;
      l->lines = (LineList) malloc(sizeof(struct LineListRec));
      l->lines->lineno = lineno;
      l->vet = vet;
      l->memloc = memloc;
      l->IType = IType;
      l->RetType = RetType;
      l->StmtType = StmtType;
      l->escopo = escopo;
      l->lines->next = NULL;
      l->next = hashTable[h];
      hashTable[h] = l;
      return;
    }
    else{
      LineList t = l->lines;
      while (t->next != NULL) t = t->next;
      t->next = (LineList) malloc(sizeof(struct LineListRec));
      t->next->lineno = lineno;
      t->next->next = NULL;
      return;
    }
  }

// Procura a ultima declaração com o mesmo nome
  while ((l != NULL) && ((strcmp(name,l->name) != 0))){
    l = l->next;
  }

  //Para inserir: não achou outra declaração, se achou verificar se o escopo é DIF e não é uma função
  if ( l == NULL || (memloc != 0 && l->escopo != escopo && l->IType != FUN && l->IType != CALL) || IType == RETT) { /* variável não está na tabela ainda */
    if(l != NULL && strcmp(l->name,name) == 0 && strcmp(l->escopo,"global") == 0){
      printf("[%d] Erro semântico! Variável '%s' já declarada no escopo global.\n",lineno,name);
      Error = TRUE;
    }else{

      l = (BucketList) malloc(sizeof(struct BucketListRec));
      l->name = name;
      l->lines = (LineList) malloc(sizeof(struct LineListRec));
      l->lines->lineno = lineno;
      l->vet = vet;
      if(IType != RETT) l->memloc = memloc;
      else l->memloc = -1;
      l->IType = IType;
      l->RetType = RetType;
      l->StmtType = StmtType;
      l->escopo = escopo;
      l->lines->next = NULL;
      l->next = hashTable[h];
      hashTable[h] = l;
      if(IType == PVET || IType == PVAR) insereParam(escopo);
    }
  }
  else if( (l->IType == FUN  && IType == VAR) || (l->IType == CALL  && IType == VAR)){
    printf("[%d] Erro semântico! Nome da variável '%s' já utilizada como nome de função.\n",lineno,name);
    Error = TRUE;
  }
  else if( l->escopo == escopo && memloc != 0 ){
    printf("[%d] Erro semântico! Variável '%s' já declarada neste escopo.\n",lineno,name);
    Error = TRUE;
    
  }else if(l->escopo != escopo){
    while ((l != NULL)){
      if((strcmp(l->escopo, "global")==0)&& ((strcmp( name,l->name) == 0))){
        LineList t = l->lines;
        while (t->next != NULL) t = t->next;
        t->next = (LineList) malloc(sizeof(struct LineListRec));
        t->next->lineno = lineno;
        t->next->next = NULL;
        break;
      }
      l = l->next;
    }
    if(l == NULL){
      printf("[%d] Erro semântico! Variável '%s' não declarada neste escopo.\n",lineno,name);
      Error = TRUE;
    }
  }
  else if(memloc == 0){
    LineList t = l->lines;
    if((l->IType != VAR && l->IType != PVAR && l->IType != PVET) && IType == VAR){
      printf("[%d] Erro semântico! Vetor '%s' usado como variável.\n",lineno,name);
      Error = TRUE;
      return;
    }else if((l->IType != VET && l->IType != PVET) && IType == VET){
      printf("[%d] Erro semântico! Variável '%s' usada como vetor.\n",lineno,name);
      Error = TRUE;
      return;
    }
    while (t->next != NULL) t = t->next;
    t->next = (LineList) malloc(sizeof(struct LineListRec));
    t->next->lineno = lineno;
    t->next->next = NULL;
  }
}
// Verifica se ja foi inserido na tabela de simbolos dado o nome e o escopo da variavel
int st_lookup ( char * name, char * escopo){
  int h = hash(name);
  BucketList l =  hashTable[h];
  while ((l != NULL) && (strcmp(name,l->name) != 0) && (strcmp(escopo,l->escopo) != 0)){
    l = l->next;}
  if (l == NULL) return -1;
  else {
    return 0;
    }
}

//Garante que sempre vai ter a função main, pois retorna erro se não tiver 
void busca_main () {
  int h = hash("main");
  BucketList l =  hashTable[h];
  while ((l != NULL) && ((strcmp("main",l->name) != 0 || l->IType == VAR)))
    l = l->next;
  if (l == NULL) {
    printf("     Erro semântico! A Função main não declarada\n");
    Error = TRUE;
  }
}

dataTypes getFunStmt(char* nome){
  int h = hash(nome);
  BucketList l =  hashTable[h];
  while ((l != NULL)){
    if (strcmp(nome,l->name) == 0){
      if (l->IType == FUN) break;
    }
    l = l->next;
  }
  if (l == NULL) return -1;
  else return l->StmtType;
}

int checkReturn(char* escopo){
  char nome[6] = "return";
  int h = hash(nome);
  BucketList l =  hashTable[h];
  while ((l != NULL)){
    if (strcmp(nome,l->name) == 0){
      if (strcmp(escopo,l->escopo) == 0) return 1;
    }
    l = l->next;
  }
  return -1;
}

dataTypes getFunType(char* nome){
  int h = hash(nome);
  BucketList l =  hashTable[h];

  while ((l != NULL)){
    if (strcmp(nome,l->name) == 0){
      if (l->IType == FUN) break;
    }
    l = l->next;
  }
  if (l == NULL) return -1;
  else return l->RetType;
}

//Retorna a posição da variavel de tal escopo na memoria global

int getMemLoc(char* nome, char* escopo){
  int h = hash(nome);
  BucketList l =  hashTable[h];
  while ((l != NULL)){
    if (strcmp(nome,l->name) == 0){
      if(strcmp(escopo,l->escopo) == 0) break;
    }
    l = l->next;
  }
  if (l == NULL) return -1;
  else return l->memloc;
}

IDTypes getVarType(char* nome, char* escopo){

  int h = hash(nome);
  BucketList l =  hashTable[h];
  if(nome == NULL) return -1;
  while ((l != NULL)){
    if (strcmp(nome,l->name) == 0){
      if(strcmp(escopo,l->escopo) == 0) break;
    }
    l = l->next;
  }
  if (l == NULL) return -1;
  else return l->IType;

}

//Retorna o numero de parametros de uma determinada variavel
int getNumParam(char *nome){

  int h = hash(nome);
  BucketList l =  hashTable[h];
  if(nome == NULL) return -1;
  while ((l != NULL)){
    if (strcmp(nome,l->name) == 0){
      if(l->IType == FUN) break;
    }
    l = l->next;
  }
  if (l == NULL) return -1;
  else return l->tam;
}

//Indica que uma determinada função possui um parametro a mais, logo é usada para contar os parametros das funções
void insereParam(char *nome){
  int h = hash(nome);
  BucketList l =  hashTable[h];
  if(nome == NULL) return;
  while ((l != NULL)){
    if (strcmp(nome,l->name) == 0){
      if(l->IType == FUN) break;
    }
    l = l->next;
  }
  if (l == NULL) return;
  else l->tam++;
}

void printSymTab(FILE * tabela_simbolos) {
  int i;
 
  fprintf(tabela_simbolos,"Identificador    scope     TipoID      TipoData      Mem. Loc.  Num da linha\n");
  fprintf(tabela_simbolos,"-------------    ------    -------     --------      ---------  ------------\n");
  for (i=0;i<SIZE;++i) {
    if (hashTable[i] != NULL) {
      BucketList l = hashTable[i];
      while (l != NULL) {
        LineList t = l->lines;
        fprintf(tabela_simbolos,"%-14s ",l->name);
        fprintf(tabela_simbolos,"%-14s   ",l->escopo);
        char* id, *data;
        switch(l->IType){
          case RETT:
            id = "ret";
            break;
          case VAR:
            id = "var";
            break;
          case PVAR:
            id = "pvar";
            break;
          case FUN:
             id = "fun";
            break;
          case CALL:
             id = "call";
            break;
          case VET:
            id= "vet";
            break;
          case PVET:
            id= "pvet";
            break;
          default:
          break;
        }
        switch(l->RetType){
          case INTTYPE:
            data= "INT";
          break;
          case VOIDTYPE:
            data= "VOID";
          break;
          case NULLL:
            data = "null";
          break;
          default:
          break;
        }
        fprintf(tabela_simbolos,"%-10s  ",id);
        fprintf(tabela_simbolos,"%-10s  ",data);
        if(l->memloc >= 0) fprintf(tabela_simbolos,"    %-5d     ",l->memloc);
        else fprintf(tabela_simbolos,"    -       ");
        while (t != NULL) {
          fprintf(tabela_simbolos,"%3d; ",t->lineno);
          t = t->next;
        }
        fprintf(tabela_simbolos,"\n");
        l = l->next;
      }
    }
  }
} 