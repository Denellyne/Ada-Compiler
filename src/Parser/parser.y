%{
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "Ast/ast.h"
int yylex (void);
void yyerror (struct _prog** prog,char const *);
%}
%code requires{

#include "Ast/ast.h"
}
%parse-param{struct _prog** prog}
%union{
  Stm stm;
  Exp exp;
  Arg args;
  double num;
  char* str;
  int tag;

}
%type <stm> stmt stmtExpr stmtAssign function  varDec variable loop if ifBody
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
%token TOK_POW


/* Precedences*/

%left TOK_OP_MULT TOK_OP_DIV TOK_POW
%left    TOK_OR
%left  TOK_XOR  
%left TOK_AND
%left  TOK_NOTEQ TOK_EQ TOK_GREATER TOK_LESS TOK_GREATEREQ TOK_LESSEQ 
%left TOK_OP_ADD TOK_OP_MINUS
%left TOK_NOT
%left TOK_LP TOK_RP
%left TOK_END_STATEMENT


/* completar: declarações de tokens */


%%



top : varDec TOK_BEGIN stmt TOK_END TOK_MAIN TOK_END_STATEMENT  { printf("\nPrinting AST:\n");*prog=mkProg($1,$3);printProg(*prog);printf("\n");}
    ;

varDec : TOK_PROC TOK_MAIN TOK_IS variable {$$ = $4;}
       ;



variable : TOK_ID TOK_COLON varTypes TOK_ASSIGN expr TOK_END_STATEMENT variable {$$ = mkCompound(mkAssign($1,$3,$5),$7);}
         | TOK_ID TOK_COLON varTypes TOK_END_STATEMENT variable {$$ = mkCompound(mkAssign($1,$3,NULL),$5);}
         | stmtAssign variable {$$ = mkCompound($1,$2);}
         | %empty {$$ = NULL;}
         ;

varTypes : TOK_STRING {$$ = 4;}
         | TOK_INTEGER {$$ = 1;}
         | TOK_BOOL {$$ = 3;}
         ;

stmt : stmtExpr stmt  { $$ = mkCompound($1,$2);}  
     | stmtExpr   { $$ = $1;}  
     ;

stmtExpr : stmtAssign {$$ = $1;}
         | function {$$ = $1;}
         | loop {$$ = $1;}
         ;  

stmtAssign : TOK_ID TOK_ASSIGN expr TOK_END_STATEMENT{$$ = mkAssign($1,-1,$3);}
           ;

function : TOK_ID TOK_LP args TOK_RP TOK_END_STATEMENT {$$ = mkFuncCall($1,$3);}
         ;

args : %empty {$$ = NULL;}
     | expr TOK_COMMA args {$$ = appendArg(mkArg($1),$3);}
     | expr {$$ = mkArg($1);}
     ;

loop : TOK_WHILE expr TOK_LOOP stmt TOK_END TOK_LOOP TOK_END_STATEMENT { $$ = mkWhile($2,$4);}
     | if {$$=$1;}
     ;

if : TOK_IF expr TOK_THEN stmt ifBody TOK_ELSE stmt TOK_END TOK_IF TOK_END_STATEMENT{ $$ = mkIf($2,$4,$5,$7);}
   | TOK_IF expr TOK_THEN stmt ifBody TOK_END TOK_IF TOK_END_STATEMENT{ $$ = mkIf($2,$4,$5,NULL);}
   ;

ifBody : %empty {$$ = NULL;}
       | TOK_ELSEIF expr TOK_THEN stmt ifBody {$$ = mkIf($2,$4,$5,NULL);}
       ;


expr : term {$$ =$1;}
     | expr TOK_OP_ADD expr {$$ = mkBinOp($1,PLUS,$3);}
     | expr TOK_OP_MINUS expr {$$ = mkBinOp($1,MINUS,$3);}
     | expr TOK_OP_MULT expr {$$ = mkBinOp($1,TIMES,$3);}
     | expr TOK_POW expr {$$ = mkBinOp($1,POW,$3);}
     | expr TOK_OP_DIV expr {$$ = mkBinOp($1,DIV,$3);}
     | expr TOK_GREATER expr {$$ = mkBinOp($1,GT,$3);}
     | expr TOK_GREATEREQ expr {$$ = mkBinOp($1,GE,$3);}
     | expr TOK_LESS expr {$$ = mkBinOp($1,LT,$3);}
     | expr TOK_LESSEQ expr {$$ = mkBinOp($1,LE,$3);}
     | expr TOK_XOR expr {$$ = mkBinOp($1,XOR,$3);}
     | expr TOK_AND expr {$$ = mkBinOp($1,AND,$3);}
     | expr TOK_OR expr {$$ = mkBinOp($1,OR,$3);}
     | expr TOK_NOTEQ expr {$$ = mkBinOp($1,NEQ,$3);}
     | expr TOK_EQ expr {$$ = mkBinOp($1,EQ,$3);}
     | TOK_OP_ADD expr {$$ = mkUnaryOp($2,PLUS);}
     | TOK_OP_MINUS expr {$$ = mkUnaryOp($2,MINUS);}
     | TOK_NOT expr {$$ = mkUnaryOp($2,NOT);}
     ;

term : TOK_NUM {$$ = mkNum($1);}
     | TOK_LP expr TOK_RP {$$ = $2;}
     | TOK_TRUE {$$ = mkBool(1);}
     | TOK_FALSE {$$ = mkBool(0);}
     | TOK_ID {$$ = mkId($1);}
     | TOK_STRLITERAL {$$ = mkStringLiteral($1);}
     ;

%%

void yyerror(struct _prog** prog,char const *msg) {
   printf("parse error: %s\n", msg);
   exit(-1);
}

