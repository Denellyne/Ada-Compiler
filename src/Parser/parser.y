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
  Arg args;
  double num;
  char* str;
  int tag;

}
%type <stm> stmt stmtExpr stmtAssign function  varDec variable loop
%type <exp> expr term 
%type <args> args
%type <tag> varTypes
// %define api.value.type {double}
%token <num> TOK_NUM 
%token <str> TOK_ID
%token <str> TOK_STRLITERAL

%token TOK_BEGIN
%token TOK_END
%token TOK_PROC
%token TOK_IS
%token TOK_MAIN
%token TOK_PUTLINE
%token TOK_GETLINE

%token TOK_IF
%token TOK_ELSE
%token TOK_ELSEIF
%token TOK_THEN
%token TOK_WHILE
%token TOK_LOOP

%token TOK_LP
%token TOK_RP
%token TOK_ASSIGN
%token TOK_LESSEQ
%token TOK_GREATEREQ
%token TOK_LESS
%token TOK_GREATER
%token TOK_EQ
%token TOK_NOTEQ
%token TOK_NOT
%token TOK_OR
%token TOK_AND
%token TOK_XOR
%token TOK_COMMA
%token TOK_COLON
%token TOK_END_STATEMENT

%token TOK_INTEGER
%token TOK_STRING
%token TOK_BOOL

%token TOK_TRUE
%token TOK_FALSE

%token TOK_OP_ADD
%token  TOK_OP_MINUS
%token  TOK_OP_MULT
%token TOK_OP_DIV


/* Precedences*/

%left TOK_OP_ADD TOK_OP_MINUS '+' '-'
%left TOK_OP_MULT TOK_OP_DIV '*' '/'
%left TOK_END_STATEMENT

/* completar: declarações de tokens */


%%


top : varDec TOK_BEGIN stmt TOK_END TOK_MAIN TOK_END_STATEMENT  { printf("Printing AST:\n");printStm(mkCompound($1,$3));printf("\n"); }
    ;

varDec : TOK_PROC TOK_MAIN TOK_IS variable {$$ = $4;}
       ;



variable : TOK_ID TOK_COLON varTypes TOK_ASSIGN expr TOK_END_STATEMENT variable {$$ = mkCompound(mkAssign($1,$3,$5),$7);}
         | TOK_ID TOK_COLON varTypes  TOK_END_STATEMENT variable {$$ = mkCompound(mkAssign($1,$3,NULL),$5);}
         | stmtAssign variable {$$ = mkCompound($1,$2);}
         | %empty {$$ = NULL;}
         ;

varTypes : TOK_STRING {$$ = 3;}
         | TOK_INTEGER {$$ = 1;}
         | TOK_BOOL {$$ = 1;}
         ;

stmt : stmtExpr stmt  { $$ = mkCompound($1,$2);}  
     | stmtExpr   { $$ = $1;}  
     ;

stmtExpr : stmtAssign {$$ = $1;}
         | function {$$ = $1;}
         // | loop {$$ = $1;}
         ;  

stmtAssign : TOK_ID TOK_ASSIGN expr TOK_END_STATEMENT{$$ = mkAssign($1,-1,$3);}
           ;

function : TOK_ID TOK_LP args TOK_RP TOK_END_STATEMENT {$$ = mkFuncCall($1,$3);}
         | TOK_PUTLINE TOK_LP args TOK_RP TOK_END_STATEMENT {$$ = mkFuncCall("Put_Line",$3);}
         | TOK_GETLINE TOK_LP args TOK_RP TOK_END_STATEMENT {$$ = mkFuncCall("Get_Line",$3);}
         ;

args : %empty {$$ = NULL;}
     | term TOK_COMMA args {$$ = appendArg(mkArg($1),$3);}
     | term {$$ = mkArg($1);}
     ;



loop : %empty {$$ = NULL;}
     ;

expr : term {$$ =$1;}
     | expr TOK_OP_ADD expr {$$ = mkBinOp($1,PLUS,$3);}
     | expr TOK_OP_MINUS expr {$$ = mkBinOp($1,MINUS,$3);}
     | expr TOK_OP_MULT expr {$$ = mkBinOp($1,TIMES,$3);}
     | expr TOK_OP_DIV expr {$$ = mkBinOp($1,DIV,$3);}
     ;

term : TOK_NUM {$$ = mkNum($1);}
     | TOK_LP expr TOK_RP {$$ = $2;}
     | TOK_TRUE {$$ = mkNum(1);}
     | TOK_FALSE {$$ = mkNum(0);}
     | TOK_ID {$$ = mkId($1);}
     | TOK_STRLITERAL {$$ = mkStringLiteral($1);}
     ;



%%

void yyerror(char const *msg) {
   printf("parse error: %s\n", msg);
   exit(-1);
}

