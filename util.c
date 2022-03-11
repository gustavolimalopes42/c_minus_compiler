#define INDENT indentno+=2
#define UNINDENT indentno-=2

#include "globals.h"
#include "util.h"
#include "codintermedgen.h"
#include "symtab.h"
#include "assembly.h"
#include "binary.h"

static int indentno = 0;

 void printToken( int token ) { 

   switch (token){ 
     case IF:   fprintf(scannerout,"IF ");break;
     case RETURN:  fprintf(scannerout,"RETURN ");break;
     case ELSE: fprintf(scannerout,"ELSE ");break;
     case INT:  fprintf(scannerout,"INT  ");break;
     case WHILE:  fprintf(scannerout,"WHILE  ");break;
     case VOID: fprintf(scannerout,"VOID ");break;
     case EQ: fprintf(scannerout,"EQ "); break;
     case ASSIGN: fprintf(scannerout,"ASSIGN "); break;
     case NEQ: fprintf(scannerout,"NEQ "); break;
     case LT: fprintf(scannerout,"LT "); break;
     case GT: fprintf(scannerout,"GT "); break;
     case LET: fprintf(scannerout,"LET "); break;
     case GET: fprintf(scannerout,"GET "); break;
     case LPAR: fprintf(scannerout,"LPAR "); break;
     case RPAR: fprintf(scannerout,"RPAR "); break;
     case LCO: fprintf(scannerout,"LCO "); break;
     case RCO: fprintf(scannerout,"RCO "); break;
     case LCHA: fprintf(scannerout,"LPAR \n"); break;
     case RCHA: fprintf(scannerout,"RPAR \n"); break;
     case END: fprintf(scannerout,"END \n"); break;
     case COMMA: fprintf(scannerout,"COMMA "); break;
     case ADD: fprintf(scannerout,"ADD "); break;
     case SUB: fprintf(scannerout,"SUB "); break;
     case MUL: fprintf(scannerout,"MUL "); break;
     case DIV: fprintf(scannerout,"DIV "); break;
     case FIM: fprintf(scannerout,"ENDFILE "); break;
     case NUM: fprintf(scannerout,"NUM ");break;
     case ID:  fprintf(scannerout,"ID ");break;
     case ERR:fprintf(scannerout,"ERROR ");break;
     default: ;
   }
 }

TreeNode * newStmtNode(StatementKind kind)
{ TreeNode * t = (TreeNode *) malloc(sizeof(TreeNode));
  int i;
  if (t==NULL)
    printf("Out of memory error at line %d\n",lineno);
  else {
    for (i=0;i<MAXCHILDREN;i++) t->child[i] = NULL;
    t->sibling = NULL;
    t->nodekind = statementK;
    t->kind.stmt = kind;
    t->lineno = lineno;
  }
  return t;
}


TreeNode * newExpNode(ExpKind kind){ 
  TreeNode * t = (TreeNode *) malloc(sizeof(TreeNode));
  int i;
  if (t==NULL)
    printf("Out of memory error at line %d\n",lineno);
  else {
    for (i=0;i<MAXCHILDREN;i++) t->child[i] = NULL;
    t->sibling = NULL;
    t->nodekind = expK;
    t->kind.exp = kind;
    t->lineno = lineno;
    t->type = Void;
  }
  return t;
}

char * copyString(char * s)
{ int n;
  char * t;
  if (s==NULL) return NULL;
  n = strlen(s)+1;
  t = malloc(n);
  if (t==NULL)
    printf("Out of memory error at line %d\n",lineno);
  else strcpy(t,s);
  return t;
}


/* printSpaces indents by printing spaces */
static void printSpaces(FILE * arvore_sintatica){ 
  int i;
  for (i=0;i<indentno;i++){
    fprintf(arvore_sintatica," ");
  }
}

 void printTree( TreeNode * tree ) { 
   int i;
   INDENT;
   while (tree != NULL) {
     printSpaces(arvore_sintatica);

     if (tree->nodekind==statementK){ 
       switch (tree->kind.stmt) {
         case ifK:
           fprintf(arvore_sintatica,"If\n");
           break;
         case whileK:
           fprintf(arvore_sintatica,"While\n");
           break;
         case assignK:
           fprintf(arvore_sintatica,"Assign: \n");
           break;
         case returnINT:
           fprintf(arvore_sintatica,"Return\n");
           break;
          case returnVOID:
           fprintf(arvore_sintatica,"Return\n");
           break;
         default:
           fprintf(arvore_sintatica,"Unknown\n");
           break;
       }
     }
     else if (tree->nodekind==expK){
      switch (tree->kind.exp) {
         case opK:
           fprintf(arvore_sintatica,"Operation: ");
           switch (tree->attr.op){
                  case EQ: fprintf(arvore_sintatica,"==\n"); break;
                  case NEQ: fprintf(arvore_sintatica,"!=\n"); break;
                  case LT: fprintf(arvore_sintatica,"<\n"); break;
                  case GT: fprintf(arvore_sintatica,">\n"); break;
                  case LET: fprintf(arvore_sintatica,"<=\n"); break;
                  case GET: fprintf(arvore_sintatica,">=\n"); break;
                  case ADD: fprintf(arvore_sintatica,"+\n"); break;
                  case SUB: fprintf(arvore_sintatica,"-\n"); break;
                  case MUL: fprintf(arvore_sintatica,"*\n"); break;
                  case DIV: fprintf(arvore_sintatica,"/\n"); break;
           }
           break;
         case constK:
           fprintf(arvore_sintatica,"constant: %d\n",tree->attr.val);
           break;
         case idK:
           fprintf(arvore_sintatica,"Id: %s\n",tree->attr.name);
           break;
         case vardeclK:
           fprintf(arvore_sintatica,"Variable: %s\n",tree->attr.name);
           break;
         case functionK:
           fprintf(arvore_sintatica,"Function: %s\n",tree->attr.name);
           break;
         case callK:
           fprintf(arvore_sintatica,"Call to Function: %s\n",tree->attr.name);
           break;
         case typeK:
           fprintf(arvore_sintatica,"Type: %s\n",tree->attr.name);
           break;
          case varparamK:
           fprintf(arvore_sintatica,"Parameter: %s\n",tree->attr.name);
           break; 
          case vetparamK:
           fprintf(arvore_sintatica,"Parameter: %s\n",tree->attr.name);
           break; 
          case vetorK:
          fprintf(arvore_sintatica, "Vector: %s", tree->attr.name);
	        break;
          default:
           fprintf(arvore_sintatica,"Unknown: %d\n", tree->nodekind);
           break;
       }
     }
     else fprintf(arvore_sintatica,"Unknown\n");
     for (i=0;i<MAXCHILDREN;i++)
          printTree(tree->child[i]);
     tree = tree->sibling;
   }
   
   UNINDENT;
 }
