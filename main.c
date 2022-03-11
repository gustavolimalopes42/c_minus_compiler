#include "globals.h"
#include "util.h"
#include "scanner.h"
#include "parse.h"
#include "analyze.h"
#include "codintermedgen.h"
#include "assembly.h"
#include "binary.h"

extern FILE *yyin;
extern int check_return;
int lineno = 1;

FILE *input_code;
FILE *scannerout; //Arquivo com os Tokens da Analise Lexica
FILE *arvore_sintatica; //Arquivo com a Arvore de Análise sintática
FILE *tabela_simbolos; //Arquivo com a Tabela de Simbolos 
FILE *quadruplas;  //Arquivo com as Quadrulhas do Codigo Intermediario
FILE *assembly_code;  //Arquivo com as Instruções Assembly do Codigo Assembly
FILE *binary_code;  //Arquivo com as Palavras de 32 bits do Codigo Binário


TreeNode *arv_sintatica;

int Error = FALSE; 


int main( int argc, char * argv[] ) {

  input_code = fopen("input_code.txt", "r");                       //Arquivo de entrada a ser compilado

  scannerout = fopen("scanner_tokens.out", "w+");                  //Arquivo com os Tokens da Analise Lexica
  arvore_sintatica = fopen("arvore_analise_sintatica.out", "w+"); //Arquivo com a Arvore de Análise sintática
  tabela_simbolos = fopen("tabela_simbolos.out", "w+");           //Arquivo com a Tabela de Simbolos 
  quadruplas = fopen("quadruplas.out", "w+");                     //Arquivo com as Quadrulhas do Codigo Intermediario
  assembly_code = fopen("assembly_code.out", "w+");              //Arquivo com as Instruções Assembly do Codigo Assembly
  binary_code = fopen("binary_code.out", "w+");                   //Arquivo com as Palavras de 32 bits do Codigo Binário

  yyin = input_code;

  //Inicio da Fáse de Análise
  printf("\n---------------------------------------------------------------\n");
  printf("Início do Processo de Compilação");
  printf("\n---------------------------------------------------------------\n");
  printf("\nRodando a Fáse de Análise...\n");

  arv_sintatica = parse(); // Chamada da Análise Sintática e Criação da Arvore de Análise Sintática
  printTree(arv_sintatica); //Impressão da Árvore de Análise Sintática
  buildSymtab(arv_sintatica); //Criação e Impressão da Tábela de Símbolos

  if (!Error){
        printf("\nSem erros léxicos, sintáticos e semânticos encontrados\n");
        printf("Fase de Análise Conclúida");
  }else{
      printf("\nA Compilação não pode ser concluida\n");
      return -1;
  }

//Fim da Fáse de Análise

printf("\n---------------------------------------------------------------\n");
//Inicio da Fáse de Síntese
printf("\nRodando a Fáse de Síntese...\n");

printf("\nGerando Código Intermediário\n");
codeIntermedGen(arv_sintatica);  
printf("\nCódigo Intermediário Gerado com sucesso\n");

printf("\nGerando Código Assembly\n");
generateAssembly(getIntermediate()); 
printf("\nCódigo Assembly Gerado com sucesso\n");

printf("\nGerando Código Binário\n");
generateBinary();  
printf("\nCódigo Binário Gerado com sucesso\n");

//Fim da Fase de Análise 
printf("\n---------------------------------------------------------------\n");

printf("Compilação concluida com sucesso!");
printf("\n---------------------------------------------------------------\n");


fclose(input_code);
fclose(scannerout);
fclose(arvore_sintatica);
fclose(tabela_simbolos);
fclose(quadruplas);
fclose(assembly_code);
fclose(binary_code);

return 0;
}