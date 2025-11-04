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

}
%type <stm> stmt stmtExpr stmtAssign function varDec variable
%type <exp> expr term types 
%type <args> args
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

variable : TOK_ID TOK_COLON TOK_STRING TOK_ASSIGN TOK_STRLITERAL TOK_END_STATEMENT variable {$$ = mkCompound(mkAssign($1,mkStringLiteral($5)),$7);}
         | TOK_ID TOK_COLON TOK_STRING  TOK_END_STATEMENT variable {$$ = mkCompound(mkAssign($1,mkStringLiteral("")),$5);}
         | TOK_ID TOK_COLON TOK_INTEGER TOK_ASSIGN expr TOK_END_STATEMENT variable {$$ = mkCompound(mkAssign($1,$5),$7);}
         | TOK_ID TOK_COLON TOK_INTEGER  TOK_END_STATEMENT variable {$$ = mkCompound(mkAssign($1,mkNum(0)),$5);}
         | TOK_ID TOK_COLON TOK_BOOL TOK_ASSIGN expr TOK_END_STATEMENT variable {$$ = mkCompound(mkAssign($1,$5),$7);}
         | TOK_ID TOK_COLON TOK_BOOL TOK_ASSIGN TOK_TRUE TOK_END_STATEMENT variable {$$ = mkCompound(mkAssign($1,mkNum(1)),$7);}
         | TOK_ID TOK_COLON TOK_BOOL TOK_ASSIGN TOK_FALSE TOK_END_STATEMENT variable {$$ = mkCompound(mkAssign($1,mkNum(0)),$7);}
         | TOK_ID TOK_COLON TOK_BOOL  TOK_END_STATEMENT variable {$$ = mkCompound(mkAssign($1,mkNum(0)),$5);}
         | stmtAssign variable {$$ = mkCompound($1,$2);}
         | %empty {$$ = NULL;}
         ;


stmt : stmtExpr stmt  { $$ = mkCompound($1,$2);}  
     | %empty {$$ = NULL;}
     ;

stmtExpr : stmtAssign {$$ = $1;}
         | function {$$ = $1;}
         // | loop {$$ = $1;}
         ;  

stmtAssign : TOK_ID TOK_ASSIGN expr TOK_END_STATEMENT{$$ = mkAssign($1,$3);}
           ;

function : TOK_ID TOK_LP args TOK_RP TOK_END_STATEMENT {$$ = mkFuncCall($1,$3);}
         | TOK_PUTLINE TOK_LP args TOK_RP TOK_END_STATEMENT {$$ = mkFuncCall("Put_Line",$3);}
         | TOK_GETLINE TOK_LP args TOK_RP TOK_END_STATEMENT {$$ = mkFuncCall("Get_Line",$3);}
         ;

args : %empty {$$ = NULL;}
     | types TOK_COMMA args {$$ = appendArg(mkArg($1),$3);}
     | types {$$ = mkArg($1);}
     ;

types : TOK_STRLITERAL {$$ = mkStringLiteral($1);}
      | TOK_NUM {$$ = mkNum($1);}
      | TOK_ID {$$ = mkId($1);}
      | TOK_TRUE {$$ = mkNum(1);}
      | TOK_FALSE {$$ = mkNum(0);}
      ;

// loop : %empty {$$ = NULL;}
     // ;

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

