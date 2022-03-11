#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#ifndef YYPARSER

#include "parser.tab.h"

#define ENDFILE 0

#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#define MAXRESERVED 8
#define nregisters 64 // numero de registradores
#define nregtemp  30// numero de registradores temporários
#define nregparam 28 // numero de registradores de parametros

//A memoria de dados possui 512 endereços de 32 bits cada. De 0 a 255 é reservada para variaveis globais e de 256 a 511 é reservada para variaveis locais de uma determinada função
#define initVarGlobalAddress 0 // Endereço do inicio das variaveis globais na memoria de dados
#define initVarLocalAddress 256 // Endereço do inicio das variaveis locais na memoria de dados


extern FILE* input_code; 

extern FILE *scannerout; //Arquivo com os Tokens da Analise Lexica
extern FILE *arvore_sintatica;//Arquivo com a Arvore de Análise sintática
extern FILE *tabela_simbolos;//Arquivo com a Tabela de Simbolos 
extern FILE *quadruplas;//Arquivo com as Quadrulhas do Codigo Intermediario
extern FILE *assembly_code;  //Arquivo com as Instruções Assembly do Codigo Assembly
extern FILE *binary_code;  //Arquivo com as Palavras de 32 bits do Codigo Binário

extern int lineno; 


typedef enum 
{
  statementK,expK
}NodeKind;

typedef enum 
{
  ifK,whileK,assignK,returnINT,returnVOID

}StatementKind;

typedef enum 
{
  opK,constK,idK,vardeclK,vetdeclK,functionK,callK,typeK,vetorK,varparamK,vetparamK
} ExpKind;

//Enumerações auxiliares para analise dos tipos de expressão, dados e variaveis
typedef enum {Void,Integer,Boolean} ExpType;
typedef enum {INTTYPE, VOIDTYPE, NULLL} dataTypes;
typedef enum {VAR, PVAR, FUN, CALL, VET, PVET, RETT} IDTypes;

#define MAXCHILDREN 3

typedef struct treeNode {
    struct treeNode * child[MAXCHILDREN];
     struct treeNode * sibling;
     int lineno;
     int size;
     int add;
     int already_seem;
     NodeKind nodekind;
     union { 
       StatementKind stmt; 
       ExpKind exp;
      }kind;
     union { 
       int op; 
       int val;
       char * name; 
      } attr;
      char * idname;
      char *  scope;
      char * idtype;
      char * datatype;
      int vet;
      int declared;
      int params;
      dataTypes type; 

} TreeNode;


TreeNode * arv_sintatica;


extern int Error;

#endif