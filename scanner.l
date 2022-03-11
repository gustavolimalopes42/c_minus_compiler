%{
#include "globals.h"
#include "util.h"
#include "scanner.h"


char tokenString[40];
char id[40];
char func[40];
extern int yylex();

int flag_coment = 0;
int num_lines = 0;
int token = 0;

%}
digit [0-9]
letter [a-zA-Z]
other [^0-9a-zA-Z;/=\-"+""*""("")""{""[""}"">""<""!"",""\n""\t""\]"]
newline [\n]
whitespace [\t]
%%
"else"                  {return ELSE;}
"if"                    {return IF;}
"int"                   {return INT;}
"return"                {return RETURN;}
"void"                  {return VOID;}
"while"                 {return WHILE;}
"/*" {
    flag_coment =1; 
    while (token != -1 && token != FIM) {token = yylex();}
}
"*/"   {flag_coment =0; return -1;}
"+"				              {return ADD;}
"-"				              {return SUB;}
"*"				              {return MUL;}
"/"				              {return DIV;}
">="                    {return GET;}
"<="                    {return LET;}
"=="                    {return EQ;}
"="				              {return ASSIGN;}
">"                     {return GT;}
"<"                     {return LT;}
"!="                    {return NEQ;}
","                     {return COMMA;}
";"				              {return END;}
"("				              {return LPAR;}
")"				              {return RPAR;}
"["                     {return LCO;}
"]"                     {return RCO;}
"{"                     {return LCHA;}
"}"                     {return RCHA;}
{letter}+	              { return ID;}
{digit}+		            {return NUM;}
({letter}|{digit})+     {if(!flag_coment) return ERR;}
{newline}               { lineno ++;}
{whitespace}                   {}
<<EOF>>			  {return ENDFILE;}
[ ]*                    {}
{other}+		            {if(!flag_coment) return ERR;}
%%


//Atualizei a função getToken do Entregavel 1 para o Entregavel 2 
int getToken(void){ 

  int currentToken;

  currentToken = yylex();
  
  if(currentToken == ID) {
      strncpy(id,yytext,40);
  }
  strncpy(tokenString,yytext,40);
  
  printToken(currentToken);

  return currentToken;
  
}