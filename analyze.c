#include "globals.h"
#include "symtab.h"
#include "analyze.h"
#include <stdio.h>
#define INDIF -1


static int locationMemGlobal = initVarGlobalAddress; // inicializa com o endereço da primeira posição para variaveis globais
static int locationMemLocal = initVarLocalAddress; // inicializa com o endereço da primeira posição para variaveis locais

char* variable_scope = "global";
int check_return = FALSE;


/* Procedure traverse is a generic recursive syntax tree traversal routine: it applies preProc in
preorder and postProc in postorder to tree pointed to by t */
static void traverse( TreeNode * t, void (* preProc) (TreeNode *), void (* postProc) (TreeNode *) )
{
  if (t != NULL){ 
    UpdateScope(t);
    preProc(t);
    { int i;
      for (i=0; i < MAXCHILDREN; i++)
        traverse(t->child[i],preProc,postProc);
    }
    if(t->child[0]!= NULL && t->kind.exp == functionK) variable_scope = "global";
    postProc(t);
    traverse(t->sibling,preProc,postProc);
  }
}
/* nullProc is a do-nothing procedure to
 * generate preorder-only or postorder-only
 * traversals from traverse
 */
static void nullProc(TreeNode * t){ 
  if (t==NULL) return;
  else return;
}

void UpdateScope(TreeNode * t){
  if (t != NULL && t->kind.exp == functionK){
    variable_scope = t->attr.name;
    if(getFunType(variable_scope) == INTTYPE && check_return == TRUE){
      if(checkReturn(variable_scope) == -1){
        printf("[%d] Erro Semântico! Retorno da função '%s' inexistente.\n",t->lineno,variable_scope);
        Error = TRUE;
      }
    }
  }
}



/* O procedimento insertNode insere identificadores armazenados em t na tabela de símbolos */
static void insertNode( TreeNode * t) {
  dataTypes TIPO = NULLL;
  switch (t->nodekind){
    case statementK:
      switch (t->kind.stmt){

      case returnVOID:
        if(getFunType(variable_scope) == INTTYPE){
          printf("[%d] ERRO SEMÂNTICO! Retorno da função '%s' incompatível.\n",t->lineno,variable_scope);
          Error = TRUE;
        }
        st_insert("return",t->lineno,0,variable_scope,INTTYPE, NULLL, RETT, t->vet); 
        break;

      case returnINT:
        if(getFunType(variable_scope) == VOIDTYPE){
          printf("[%d] ERRO SEMÂNTICO! Retorno da função '%s' incompatível.\n",t->lineno,variable_scope);
          Error = TRUE;
        }
        st_insert("return",t->lineno,0,variable_scope,INTTYPE, NULLL, RETT, t->vet); 
        break;
      default:
        break;
      }
      break; 

    case expK:
      switch(t->kind.exp){

        case vardeclK:

          /* não encontrado na tabela, inserir*/
          
            if(variable_scope == "global"){

              st_insert(t->attr.name,t->lineno,locationMemGlobal++,variable_scope,INTTYPE, TIPO, VAR, t->vet);
            }else{
            st_insert(t->attr.name,t->lineno,locationMemLocal++,variable_scope,INTTYPE, TIPO, VAR, t->vet);
            }
          
          break;

        case vetorK:

            if(variable_scope == "global"){

              st_insert(t->attr.name,t->lineno,locationMemGlobal++,variable_scope,INTTYPE, TIPO, VAR, t->vet);
              locationMemGlobal += t->child[1]->attr.val;
            }else{
            st_insert(t->attr.name,t->lineno,locationMemLocal++,variable_scope,INTTYPE, TIPO, VAR, t->vet);
              locationMemLocal += t->child[1]->attr.val;
            }

        break;

        case functionK:

        locationMemLocal = initVarLocalAddress;
        
          if(strcmp(t->child[1]->attr.name,"VOID") == 0) TIPO = VOIDTYPE;
          else TIPO = INTTYPE;
          if (st_lookup(t->attr.name,variable_scope) == -1){
          
            st_insert(t->attr.name,t->lineno,INDIF, "global",t->type, TIPO,FUN, t->vet);}
          else{
            printf("[%d] ERRO SEMÂNTICO: Múltiplas declarações da função '%s'.\n",t->lineno,t->attr.name);
            Error = TRUE;
          }
          break;

        case varparamK:
            st_insert(t->attr.name,t->lineno,locationMemLocal++,variable_scope,INTTYPE, TIPO, PVAR, t->vet);
          break;

        case vetparamK:
            st_insert(t->attr.name,t->lineno,locationMemLocal++,variable_scope,INTTYPE, TIPO, PVET, t->vet);
          break;

        case idK:
          if(t->add != 1){
            if (st_lookup(t->attr.name, variable_scope) == -1){
              printf("[%d] ERRO SEMÂNTICO! A variável '%s' não foi declarada.\n",t->lineno,t->attr.name);
              Error = TRUE;
            }
            else {
              if(t->child[0] != NULL)
                st_insert(t->attr.name,t->lineno,0,variable_scope,INTTYPE, TIPO, VET, t->vet);
              else
                st_insert(t->attr.name,t->lineno,0,variable_scope,INTTYPE, TIPO, VAR, t->vet);
            }
          }
          break;

        case callK:
          if (st_lookup(t->attr.name, variable_scope) == -1 && (strcmp(t->attr.name, "input") != 0) && (strcmp(t->attr.name, "output") != 0)){
            printf("[%d] ERRO SEMÂNTICO! A função '%s' não foi declarada.\n",t->lineno,t->attr.name);
            Error = TRUE;
          }
          else {
            if(t->params == getNumParam(t->attr.name) || strcmp(t->attr.name,"output")==0 || strcmp(t->attr.name,"input")==0)
              st_insert(t->attr.name,t->lineno,INDIF,variable_scope,getFunType(t->attr.name), TIPO,CALL, t->vet);

            else{
              printf("[%d] ERRO SEMÂNTICO! Número de parâmetros para a função '%s' incompatível.\n",t->lineno,t->attr.name);
              Error = TRUE;
            }
          }
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}

/* Function buildSymtab constructs the symbol
 * table by preorder traversal of the syntax tree
 */
void buildSymtab(TreeNode * arv_sintatica){

  traverse(arv_sintatica,insertNode,nullProc);
  busca_main(); //Garante que vai ter a função main, se n alerta erro semântico
  check_return = TRUE;
  typeCheck(arv_sintatica);

  printSymTab(tabela_simbolos);
    
}

static void typeError(TreeNode * t, char * message){
  printf("[%d] %s\n",t->lineno,message);
  Error = TRUE;
}

/* Procedure checkNode performs
 * type checking at a single tree node
 */

void checkNode(TreeNode * t){
  switch (t->nodekind){
  case expK:
      switch (t->kind.exp){
      case opK:
        if((t->child[0] == NULL) || (t->child[1] == NULL)) break;
        if (((t->child[0]->kind.exp == callK) &&( getFunType(t->child[0]->attr.name)) == VOIDTYPE) ||
            ((t->child[1]->kind.exp == callK) && (getFunType(t->child[1]->attr.name) == VOIDTYPE)))
              typeError(t,"ERRO SEMÂNTICO! Uma funcao com retorno VOID não pode ser um operando.");
        break;
      case callK:
        if (((t->params > 0) && (getFunStmt(t->attr.name)) == VOIDTYPE))
              typeError(t,"ERRO SEMÂNTICO! Insercao de parametros a uma função do tipo VOID.");
        break;
        default:
          break;
      }
      break;
    case statementK:
      switch (t->kind.stmt){
        case assignK:
          if((t->child[1] == NULL)) break;
          if (t->child[1]->kind.exp == callK && getFunType(t->child[1]->attr.name) == VOIDTYPE)
            typeError(t,"ERRO SEMÂNTICO! Uma funcao com retorno VOID não pode ser atribuida a uma variavel.");
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}

/* Procedure typeCheck performs type checking
 * by a postorder syntax tree traversal
 */
void typeCheck(TreeNode * arv_sintatica){
  traverse(arv_sintatica,checkNode, nullProc);

}