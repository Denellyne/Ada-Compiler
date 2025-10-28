%{
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
int yylex (void);
void yyerror (char const *);
%}
%code requires{

#include "Ast/ast.h"
}
%union{
  Stm stm;
  Exp exp;
  double num;
  char* str;
}
%type <stm> stmt stme 
%type <exp> expr term 
// %define api.value.type {double}
%token <num> TOK_NUM
%token <str> TOK_ID
%token TOK_ASSIGN
%token TOK_INCR
%token TOK_OP_ADD
%token  TOK_OP_MINUS
%token  TOK_OP_MULT
%token TOK_OP_DIV
%token TOK_END_STATEMENT
%token   TOK_LP
%token   TOK_RP
%token TOK_BEGIN
%token TOK_END
%token TOK_PROC
%token TOK_IS
%token TOK_MAIN
%token TOK_PUTLINE

%left TOK_OP_ADD TOK_OP_MINUS '+' '-'
%left TOK_OP_MULT TOK_OP_DIV '*' '/'
%left TOK_END_STATEMENT

/* completar: declarações de tokens */


%%

top : TOK_PROC TOK_MAIN TOK_IS TOK_BEGIN stmt TOK_END TOK_MAIN TOK_END_STATEMENT  { printf("Printing AST:\n");printStm($5); }
    ;

stmt : stme stmt  { $$ = mkCompound($1,$2);}  
     | stme {$$ = $1;}
     ;
stme : TOK_ID TOK_ASSIGN expr TOK_END_STATEMENT{$$ = mkAssign($1,$3);}
     | TOK_ID TOK_INCR TOK_END_STATEMENT{$$ = mkIncr($1);}
     ;

expr : term {$$ =$1;}
     | expr TOK_OP_ADD expr {$$ = mkBinOp($1,PLUS,$3);}
     | expr TOK_OP_MINUS expr {$$ = mkBinOp($1,MINUS,$3);}
     | expr TOK_OP_MULT expr {$$ = mkBinOp($1,TIMES,$3);}
     | expr TOK_OP_DIV expr {$$ = mkBinOp($1,DIV,$3);}
     | TOK_ID {$$ = mkId($1);}
     ;

term : TOK_NUM {$$ = mkNum($1);}
     | TOK_LP expr TOK_RP {$$ = $2;}
     ;



%%

void yyerror(char const *msg) {
   printf("parse error: %s\n", msg);
   exit(-1);
}

