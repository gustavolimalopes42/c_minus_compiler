/****************************************************/
/* File: cgen.c                                     */
/* The code generator implementation                */
/* for the C- compiler                              */
/* Adapted from:                                    */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "globals.h"
#include "symtab.h"
#include "codintermedgen.h"
#include "parse.h"
#include "analyze.h"

/* tmpOffset is the memory offset for temps
   It is decremented each time a temp is
   stored, and incremeted when loaded again */
static int tmpOffset = 0;

/* prototype for internal recursive code generator */
static void insertQuad(TreeNode *tree);

QuadList head = NULL;

int location = 0;
int mainLocation;
int memLoc;

int nlabel = 0;
int ntemp = 1;
int nparams = -1;

Address aux;
Address var;
Address offset;
Address empty;

char var_scope[30] = "global";

const char *OpKindNames[] = {"add", "sub", "mult", "div", "blt", "lequal", "bgt", "grequal", "beq", "bne", "and", "or", "assign",
                            "alloc", "addi", "subi", "load", "store", "vector", "goto", "iff", "ret", "fun", "end",
                            "param", "call", "arg", "label", "halt", "move", "put"};


void quad_insert(OpKind op, Address addr1, Address addr2, Address addr3){
  Quad quad;
  quad.op = op;
  quad.addr1 = addr1;
  quad.addr2 = addr2;
  quad.addr3 = addr3;
  QuadList new = (QuadList)malloc(sizeof(struct QuadListRec));
  new->location = location;
  new->quad = quad;
  new->next = NULL;
  if (head == NULL){
    head = new;
  }
  else{
    QuadList q = head;
    while (q->next != NULL)
      q = q->next;
    q->next = new;
  }
  location++;
}

int quad_update(int loc, Address addr1, Address addr2, Address addr3){
  QuadList q = head;
  while (q != NULL){
    if (q->location == loc)
      break;
    q = q->next;
  }
  if (q == NULL)
    return 0;

  else{
    if(addr1.kind != Empty) q->quad.addr1 = addr1;
    if(addr2.kind != Empty) q->quad.addr2 = addr2;
    if(addr3.kind != Empty) q->quad.addr3 = addr3;
    return 1;
  }
}

char *newLabel(){
  char *label = (char *)malloc((nlabel_size + 3) * sizeof(char));
  sprintf(label, "L%d", nlabel);
  nlabel++;
  return label;
}

char *newTemp(){
  char *temp = (char *)malloc((ntemp_size + 3) * sizeof(char));
  sprintf(temp, "$r%d", ntemp);
  ntemp = (ntemp % (nregtemp-1))+1;
  return temp;
}

Address addr_createEmpty(){
  Address addr;
  addr.kind = Empty;
  addr.contents.var.name = NULL;
  addr.contents.var.scope = NULL;
  return addr;
}

Address addr_createIntConst(int val){
  Address addr;
  addr.kind = IntConst;
  addr.contents.val = val;
  return addr;
}

Address addr_createString(char *name, char *scope){
  Address addr;
  addr.kind = String;
  addr.contents.var.name = (char *)malloc(strlen(name) * sizeof(char));
  strcpy(addr.contents.var.name,name);

  if(scope == NULL){
    addr.contents.var.scope = (char *)malloc(strlen(name) * sizeof(char));
     strcpy(addr.contents.var.scope,name);

  }else{
    addr.contents.var.scope = (char *)malloc(strlen(scope)* sizeof(char));
    strcpy(addr.contents.var.scope,scope);
  }
  return addr;
}

/* O procedimento genStmt() gera código em um nó do tipo declaração */
static void genStmt(TreeNode *t){
  
  TreeNode *p1, *p2, *p3;
  Address addr1, addr2, addr3;
  Address aux1, aux2;
  int loc1, loc2, loc3;
  char *label;
  char *temp;

  switch (t->kind.stmt){
  case ifK:

    p1 = t->child[0]; //arg
    p2 = t->child[1]; //if true
    p3 = t->child[2]; //if false

    insertQuad(p1);
    addr1 = aux;
    loc1 = location-1;

    insertQuad(p2);
    loc2 = location;
    quad_insert(opGOTO, empty, empty, empty); 
   
    label = newLabel();
    quad_insert(opLABEL,addr_createString(label, var_scope), empty, empty);
 

    quad_update(loc1, empty, empty,addr_createString(label, var_scope));

    insertQuad(p3);

    if (p3 != NULL){
    
      loc3 = location;

    }
    label = newLabel();
  
    quad_insert(opLABEL,addr_createString(label, var_scope), empty, empty);
    quad_update(loc2,addr_createString(label, var_scope), empty, empty);
    if (p3 != NULL)
      quad_update(loc3,addr_createString(label, var_scope), empty, empty);
 
    break;

  case whileK:
   
  
    p1 = t->child[0]; 
    p2 = t->child[1]; 
    
    label = newLabel();
    quad_insert(opLABEL,addr_createString(label, var_scope), empty, empty); 
    
    insertQuad(p1);
    addr1 = aux;
    
    loc1 = location-1;

    insertQuad(p2); 
    loc3 = location;
    quad_insert(opGOTO,addr_createString(label, var_scope), empty, empty); 

    label = newLabel();
    quad_insert(opLABEL,addr_createString(label, var_scope), empty, empty); 

    quad_update(loc1,empty,empty,addr_createString(label, var_scope)); 
  
 
    break;

  case assignK:
  
     
    p1 = t->child[0]; 
    p2 = t->child[1]; 
    
    insertQuad(p1);
    addr1 = aux;
    aux1 = var;
    aux2 = offset;
    
    insertQuad(p2);
    addr2 = aux;
    quad_insert(opASSIGN, addr1, addr2, empty);
    quad_insert(opSTORE, addr1, aux1, aux2);
  
    
    break;

  case returnINT:
 
    
    p1 = t->child[0];
    insertQuad(p1);
    addr1 = aux;
    quad_insert(opRET, addr1, empty, empty);

  
    break;

  case returnVOID:
 
 
    addr1 = empty;
    quad_insert(opRET, empty, empty, empty);

    break;

  default:
    break;
  }
} /* genStmt */

/* Procedure genExp generates code at an expression node */
static void genExp(TreeNode *t){
  TreeNode *p1, *p2, *p3;
  Address addr1, addr2, addr3;
  int loc1, loc2, loc3;
  char *label;
  char *temp;
  char *s = "";

  switch (t->kind.exp){

  case constK:
 
    temp = newTemp();
    addr1 = addr_createIntConst(t->attr.val);
    aux = addr_createString(temp, var_scope);
   
    quad_insert(opPUT, aux, addr_createString("$zero", var_scope), addr1);

    break;

  case idK:

    aux = addr_createString(t->attr.name, var_scope);
    p1 = t->child[0];
    if (p1 != NULL){ 
      
      temp = newTemp();
      addr1 = addr_createString(temp, var_scope);
      addr2 = aux;
      insertQuad(p1);
      quad_insert(opVEC, addr1, addr2, aux);
      var = addr2;
      offset = aux;
      aux = addr1;
    }
    else{ 
      temp = newTemp();
      addr1 = addr_createString(temp, var_scope);
      quad_insert(opLOAD, addr1, aux, empty);
      var = aux;
      offset = empty;
      aux = addr1;
    }

    break;

  case functionK:
    strcpy(var_scope,t->attr.name);

    if (strcmp(t->attr.name, "main") == 0) {
      mainLocation = location;
    }

    if ((strcmp(t->attr.name, "input") != 0) && (strcmp(t->attr.name, "output") != 0)){

      quad_insert(opFUN, addr_createString(t->attr.name, var_scope), empty, empty);
  
      p1 = t->child[1];
      insertQuad(p1);

      p2 = t->child[2];
      insertQuad(p2);
      quad_insert(opEND, addr_createString(t->attr.name, var_scope), empty, empty);
      strcpy(var_scope,"global");
    }

    break;

  case callK:

    // é um parametro
    nparams = t->params;
    p1 = t->child[0];

    while (p1 != NULL){

      if(p1->kind.exp == idK){
        if(getVarType(p1->attr.name,var_scope) == PVET){
          temp = newTemp();
          aux = addr_createString(temp,var_scope);
          quad_insert(opADDI,aux,addr_createString("$lp",var_scope),addr_createIntConst(getMemLoc(p1->attr.name,var_scope)));
        
        }else insertQuad(p1);
      }else{
        insertQuad(p1);
      }
      quad_insert(opPARAM, aux, empty, empty);
      nparams--;
      p1 = p1->sibling;
    }

    nparams = -1;
    if(getFunType(t->attr.name) == INTTYPE){
      aux = addr_createString("$ret", var_scope);
      quad_insert(opCALL, aux, addr_createString(t->attr.name, var_scope), addr_createIntConst(t->params));

      }else{
        if(strcmp(t->attr.name,"input")==0){
          aux = addr_createString("$ret",var_scope);
          quad_insert(opCALL, empty, addr_createString("input", var_scope), addr_createIntConst(t->params));
        }else if(strcmp(t->attr.name,"output")==0)
          quad_insert(opCALL, empty, addr_createString("output", var_scope), addr_createIntConst(t->params));
        else
          quad_insert(opCALL, empty, addr_createString(t->attr.name, var_scope), addr_createIntConst(t->params));
      }

    break;

  case varparamK:

    quad_insert(opARG, addr_createString(t->attr.name, var_scope), empty, addr_createString(var_scope,var_scope));

    break;

  case vetparamK:

    quad_insert(opARG, addr_createString(t->attr.name, var_scope), empty, addr_createString(var_scope,var_scope));

    break;

  case vardeclK:
    memLoc = getMemLoc(t->attr.name,var_scope) + initVarGlobalAddress;

 
    if (memLoc >= 0){
      quad_insert(opALLOC, addr_createString(t->attr.name, var_scope), addr_createIntConst(1), addr_createString(var_scope,var_scope));
    }else{
      printf("Erro ao alocar a variável '%s'! - memLoc: %d\n",t->attr.name,memLoc);
      Error = TRUE;
      return;
    }

    break;

  case vetorK:
    memLoc = getMemLoc(t->attr.name,var_scope);
    
    if (memLoc >= 0){
      quad_insert(opALLOC, addr_createString(t->attr.name, var_scope), addr_createIntConst(t->child[1]->attr.val), addr_createString(var_scope,var_scope));
    }else{
      printf("Erro ao alocar o vetor '%s'! - memLoc: %d\n",t->attr.name,memLoc);
      Error = TRUE;
      return;
    }

    break;

  case opK:

    p1 = t->child[0];
    p2 = t->child[1];
    insertQuad(p1);
    addr1 = aux;
    insertQuad(p2);
    addr2 = aux;
    temp = newTemp();
    aux = addr_createString(temp, var_scope);

    switch (t->attr.op){
    case ADD:
      quad_insert(opADD, aux, addr1, addr2);
      break;
    case SUB:
      quad_insert(opSUB, aux, addr1, addr2);
      break;
    case MUL:
      quad_insert(opMULT, aux, addr1, addr2);
      break;
    case DIV:
      quad_insert(opDIV, aux, addr1, addr2);
      break;
    case LT:
      quad_insert(opSGE, aux, addr1, addr2);
      addr1 = addr_createString(newTemp(),var_scope);
      addr2 = addr_createIntConst(1);
      quad_insert(opPUT, addr1, addr_createString("$zero", var_scope), addr2);
      quad_insert(opBEQ,aux,addr1,empty);
      break;
    case LET:
      quad_insert(opBGT,addr1,addr2,empty);
      break;
    case GT:
      quad_insert(opSLE, aux, addr1, addr2);
      addr1 = addr_createString(newTemp(),var_scope);
      addr2 = addr_createIntConst(1);
      quad_insert(opPUT, addr1, addr_createString("$zero", var_scope), addr2);
      quad_insert(opBEQ,aux,addr1,empty);
      break;
    case GET:
      quad_insert(opBLT,addr1,addr2,empty);
      break;
    case EQ:
      quad_insert(opBNE,addr1,addr2,empty);
      break;
    case NEQ:
      quad_insert(opBEQ,addr1,addr2,empty);
      break;
    default:

      break;
    }

    break;

  default:
    break;
  }
}

/* Procedimento recursivo que gera o código intermediário pela árvore sintática */
/* Refiz essa função do entregavel 1 para o 2, senti que a recursão ficou mais polida e limpa assim*/
static void insertQuad(TreeNode *syntaxTree){

  if (syntaxTree != NULL){
    switch (syntaxTree->nodekind){
    case statementK:
      genStmt(syntaxTree);
      break;
    case expK:
      genExp(syntaxTree);
      break;
    default:
      break;
    }

    if (nparams == -1 || nparams == 0) { 
      insertQuad(syntaxTree->sibling);
    }
  }
}

void printCode(QuadList head){

  QuadList q = head;
  Address a1, a2, a3;
  while (q != NULL){

    a1 = q->quad.addr1;
    a2 = q->quad.addr2;
    a3 = q->quad.addr3;
    fprintf(quadruplas,"(%s, ", OpKindNames[q->quad.op]);

    switch (a1.kind){
    case Empty:
      fprintf(quadruplas,"_");
      break;
    case IntConst:
      fprintf(quadruplas,"%d", a1.contents.val);
      break;
    case String:
      fprintf(quadruplas,"%s", a1.contents.var.name);
      break;
    default:
      break;
    }
    fprintf(quadruplas,", ");

    switch (a2.kind){
    case Empty:
      fprintf(quadruplas,"_");
      break;
    case IntConst:
      fprintf(quadruplas,"%d", a2.contents.val);
      break;
    case String:
      fprintf(quadruplas,"%s", a2.contents.var.name);
      break;
    default:
      break;
    }
    fprintf(quadruplas,", ");

    switch (a3.kind){
    case Empty:
      fprintf(quadruplas,"_");
      break;
    case IntConst:
      fprintf(quadruplas,"%d", a3.contents.val);
      break;
    case String:
      fprintf(quadruplas,"%s", a3.contents.var.name);
      break;
    default:
      break;
    }
    fprintf(quadruplas,")\n");
    q = q->next;
  }
}

/* Procedimento que percorre a arvore sintática a fim de criar o código intermediário */
void codeIntermedGen(TreeNode *arv_sintatica){

  empty = addr_createEmpty();
  insertQuad(arv_sintatica);
  quad_insert(opHALT, empty, empty, empty);
  printCode(head); //Imprime o Código Intermediário
    
}

QuadList getIntermediate(){
  return head;
}