%{
  
#define YYPARSER

#include "globals.h"
#include "util.h"
#include "scanner.h"
#include "parse.h"

#define YYSTYPE TreeNode *
static TreeNode * savedTree; 
static int yylex();

extern int lineno;
extern int tab;

char *scope = "";
char * idtype = "";
char * datatype = "";
static char *savedname = "";
int flag = 0;
int params=0;

int yyerror(char *message);
%}


%start program
%token ELSE IF INT RETURN VOID WHILE
%token ID NUM 
%left ADD SUB MUL DIV 
%left LT LET GT GET EQ NEQ 
%token ASSIGN COMMA LPAR RPAR LCHA RCHA LCO RCO TB LINE SPACE NL
%token END FIM ERR

%%

program:  decla_seq{ savedTree = $1; }
;

decla_seq:  decla_seq decla {
            YYSTYPE t = $1;
              if (t != NULL){
                while (t->sibling != NULL)
                   t = t->sibling;
                t->sibling = $2;
                $$ = $1;
              }
              else $$ = $2;
          }| decla { $$ = $1; }
;

decla:  var_decla { $$ = $1;} | fun_decla { $$ = $1;}
;

var_decla:  tipo ID END {
            $$ = newExpNode(vardeclK);
            $$->attr.name = copyString(id);
            $$->child[0] = $1;
            $$->type = $1->type;
            $$->scope= scope;
            $$->kind.exp = vardeclK;
            $$->lineno = lineno;
          }
    | tipo fun-id LCO tam RCO END {
            $$ = newExpNode(vetorK);
            $$->attr.name = copyString(id);
            $$->child[0] = $1;
            $$->child[1] = $4;
            $$->type = $1->type;
            $$->scope= scope;
            $$->kind.exp = vetorK;
            $$->lineno = lineno;
          }
    | error {yyerror("");}
;

tam: NUM {
            $$ = newExpNode(constK);
            $$->type = INTTYPE;
            $$->attr.name = NULL;
            $$->attr.val = atoi(tokenString);
            };
tipo: INT { //Atualização na arvore do Entregavel 1 para o 2
            $$ = newExpNode(typeK);
            $$->attr.name = "INT";
            $$->type = INTTYPE;
            $$->kind.exp = typeK; }
    | VOID {
            $$ = newExpNode(typeK);
            $$->attr.name = "VOID";
            $$->type = VOIDTYPE;
            $$->kind.exp = typeK; }
;

fun-id:  ID {
            $$ = newExpNode(idK);
            $$->attr.name = copyString(id);
            $$->kind.exp = idK; }
;
fun_decla: tipo fun-id LPAR params RPAR comp_decla{
              $$ = newExpNode(functionK);
              $$->kind.exp = functionK;
              $$->attr.name = $2->attr.name;
              $$->child[0] = $1;
              $$->type = $1->type;
              $$->child[1] = $4;
              $$->child[2] = $6;
              $$->lineno = $2->lineno;
            }
;

params: VOID {
            $$ = newExpNode(typeK);
            $$->attr.name = "VOID";
            $$->size = 0;
            $$->child[0] = NULL;
            $$->lineno = lineno;
          } 
        | param_seq { $$ = $1; }
;

param_seq: param_seq COMMA param_seq {
                YYSTYPE t = $1;
                if (t != NULL){
                  while (t->sibling != NULL)
                       t = t->sibling;
                  t->sibling = $3;
                  $$ = $1;
                }
                else $$ = $3;
              } | param { $$ = $1; }
;

param: tipo ID {
          $$ = newExpNode(varparamK);
          $$->attr.name = copyString(id);
          $$->kind.exp = varparamK;
          $$->size = 0;
          $$->lineno = lineno;
          $$->type = $1->type;
          $$->child[0] = $1;
        } | tipo ID LCO RCO{
         $$ = newExpNode(vetparamK);
          $$->child[0] = $1;
          $$->attr.name = copyString(id);
          $$->kind.exp = vetparamK;
          $$->size = 0;
          $$->lineno = lineno;
          $$->type = $1->type;
          $$->child[0] = $1;
        }
;

comp_decla: LCHA local_decla stmt_seq RCHA {
                YYSTYPE t = $2;
                  if (t != NULL){
                    while (t->sibling != NULL)
                       t = t->sibling;
                    t->sibling = $3;
                    $$ = $2;
                  }
                  else $$ = $3;
              }
    | LCHA RCHA {}
    | LCHA local_decla RCHA { $$ = $2; }
    | LCHA stmt_seq RCHA { $$ = $2; }
;

local_decla: local_decla var_decla {
              YYSTYPE t = $1;
                if (t != NULL){
                  while (t->sibling != NULL)
                     t = t->sibling;
                  t->sibling = $2;
                  $$ = $1;
                }
                else $$ = $2;
            }
    | var_decla { $$ = $1; }
;

stmt_seq: stmt_seq stmt {
              YYSTYPE t = $1;
              if (t != NULL){
                while (t->sibling != NULL)
                t = t->sibling;
                t->sibling = $2;
                $$ = $1;
              }
              else $$ = $2;
            }
    | stmt { $$ = $1; }
;
 
stmt: exp_decla { $$ = $1; }
    | comp_decla { $$ = $1; }
    | sele_decla { $$ = $1; }
    | itera_decla { $$ = $1; }
    | return_decla { $$ = $1; }
;

exp_decla: exp END { $$ = $1; }
              | END {}
              ;

sele_decla: IF LPAR exp RPAR stmt {
            $$ = newStmtNode(ifK);
            $$->attr.name = "IF";
            $$->child[0] = $3;
            $$->child[1] = $5;
            $$->lineno = lineno;
            $$->kind.stmt = ifK;
          }
    | IF LPAR exp RPAR stmt ELSE stmt{
            $$ = newStmtNode(ifK);
            $$->attr.name = "IF";
            $$->child[0] = $3;
            $$->child[1] = $5;
            $$->child[2] = $7;
            $$->scope= $3->scope;
            $$->lineno = lineno;
            $$->kind.stmt = ifK;
          }
;

itera_decla: WHILE LPAR exp RPAR stmt {
          $$ = newStmtNode(whileK);
          $$->attr.name = "WHILE";
          $$->child[0] = $3;
          $$->child[1] = $5;
          $$->scope = $3->scope;
          $$->lineno = lineno;
          $$->kind.stmt = whileK;
        }
;

return_decla: RETURN exp END{
                $$ = newStmtNode(returnINT);
                $$->child[0] = $2;
                $$->lineno = lineno;
            }| RETURN END { $$ = newStmtNode(returnVOID); }
;

exp: var ASSIGN exp {
        $$ = newStmtNode(assignK);
        $$->kind.stmt = assignK;
        $$->attr.name= $1->attr.name;
        $$->scope = scope;
        $$->child[0] = $1;
        $$->child[1] = $3;
        $$->lineno = lineno;
      }| simple_exp { $$ = $1; }
;

var: ID {
        $$ = newExpNode(idK);
        $$->attr.name = copyString(id);
        $$->lineno = lineno;
        $$->child[0] = NULL;
  } |fun-id LCO exp RCO {
        $$ = newExpNode(idK);
        $$->attr.name = $1->attr.name;
        $$->child[0] = $3;
        $$->lineno = lineno;
        }
;

simple_exp: sum_exp rel sum_exp {
                  $$ = newStmtNode(assignK);
                  $$ = $2;
                  $$->child[0] = $1;
                  $$->child[1] = $3;
                  $$->scope = scope;
              } | sum_exp{ $$ = $1; }
;

rel:          LET 
              {
                $$ = newExpNode(opK);
                $$->attr.op = LET;
                $$->lineno = lineno;
              } 
            | LT {
                $$ = newExpNode(opK);
                $$->attr.op = LT;
                $$->lineno = lineno;
              }
            | GT {
                $$ = newExpNode(opK);
                $$->attr.op = GT;
                $$->lineno = lineno;
              }
            | GET {
                $$ = newExpNode(opK);
                $$->attr.op = GET;
                $$->lineno = lineno;
              }
            | EQ {
                $$ = newExpNode(opK);
                $$->attr.op = EQ;
                $$->lineno = lineno;
              }
            | NEQ {
                $$ = newExpNode(opK);
                $$->attr.op = NEQ;
                $$->lineno = lineno;
              }
;

sum_exp: sum_exp sum ope {
            $$ = $2;
            $$->child[0] = $1;
            $$->child[1] = $3;
            $$->scope = scope;
            $$->lineno = lineno;
       }| ope { $$ = $1; }
;

sum: ADD {
         $$ = newExpNode(opK);
         $$->attr.op = ADD;
         $$->lineno = lineno;
   }| SUB {
         $$ = newExpNode(opK);
         $$->attr.op = SUB;
         $$->lineno = lineno;
       }
;

ope: ope mul fac {
              $$ = $2;
              $$->scope = scope;
              $$->child[0] = $1;
              $$->child[1] = $3;
              $$->lineno = lineno;
    }| fac { $$ = $1; }
;

mul: MUL {
         $$ = newExpNode(opK);
         $$->attr.op = MUL;
         $$->lineno = lineno;
   }| DIV {
         $$ = newExpNode(opK);
         $$->attr.op = DIV;
         $$->lineno = lineno;
       }
;

fac: LPAR exp RPAR { $$ = $2; } 
     | var { $$ = $1; }
     | acti { $$ = $1; params = 0; }
     | NUM {
            $$ = newExpNode(constK);
            $$->type = INTTYPE;
            $$->attr.name = "teste";
            $$->attr.val = atoi(tokenString);
            }
;

acti: fun-id LPAR arg_seq RPAR {
          $$ = newExpNode(callK);
          $$->kind.exp = callK;
          $$->attr.name = $1->attr.name;
          $$->child[0] = $3;
          $$->params = params;
          $$->lineno = lineno;
       }| fun-id LPAR RPAR{
           $$ = newExpNode(callK);
           $$->kind.exp = callK;
           $$->attr.name = $1->attr.name;
           $$->params = params;
           $$->lineno = lineno;
         }
;

arg_seq: arg_seq COMMA exp {
              YYSTYPE t = $1;
              if (t != NULL){
                while (t->sibling != NULL)
                t = t->sibling;
                t->sibling = $3;
                params ++;
                $$ = $1;
              }
              else $$ = $3;
   }| exp { 
           params ++;
           $$ = $1; }
;

%%

//Atualizei a função de print dos erros léxicos do entregavel 1 para o 2

int yyerror(char *message){

    if(yychar == -2) {
      return 0;
    }
    Error = TRUE;

    if(yychar == ERR) {
      printf("[%d] ERRO LÉXICO: ", lineno);
    }
    else printf("[%d] ERRO SINTÁTICO: ", lineno);

    switch (yychar){
        case IF:   printf("%s\n",tokenString);break;
        case RETURN:  printf("%s\n",tokenString);break;
        case ELSE: printf("%s\n",tokenString);break;
        case INT:  printf("%s\n",tokenString);break;
        case WHILE:  printf("%s\n",tokenString);break;
        case VOID: printf("%s\n",tokenString);break;
        case EQ:  printf("==\n"); break;
        case ASSIGN:  printf("=\n"); break;
        case NEQ:  printf("!=\n"); break;
        case LT: printf("<\n"); break;
        case GT: printf(">\n"); break;
        case LET: printf("<=\n"); break;
        case GET: printf(">=\n"); break;
        case LPAR:  printf("(\n"); break;
        case RPAR:  printf(")\n"); break;
        case LCO:  printf("[\n"); break;
        case RCO:  printf("]\n"); break;
        case LCHA:  printf("{\n"); break;
        case RCHA:  printf("}\n"); break;
        case END:  printf(";\n"); break;
        case COMMA: printf(",\n"); break;
        case ADD:  printf("+\n"); break;
        case SUB:  printf("-\n"); break;
        case MUL:  printf("*\n"); break;
        case DIV:  printf("/\n"); break;
        case FIM:  printf("EOF\n"); break;
        case NUM:  printf("NUM, valor = %s\n",tokenString);break;
        case ID:   printf("ID, lexema = %s\n",tokenString);break;
        case ERR:  printf("%s\n",tokenString);break;
        default: /* should never happen */
          printf(" Unknown Token: %d\n",yychar);
   }

   Error = TRUE;
    return 0;
}

static int yylex(){ 
  return getToken(); 
}

TreeNode * parse(void){ 
  yyparse();
  return savedTree;
}