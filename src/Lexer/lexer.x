%{
#include "parser.tab.h"

%}
%option noyywrap

digit   [0-9]
alpha [_a-zA-Z]


/* completar: declarações */

%%

[ \t\n\r]+



"begin" {printf("%s\n",yytext); return TOK_BEGIN;}
"end" {printf("%s\n",yytext); return TOK_END;}
"procedure" {printf("%s\n",yytext); return TOK_PROC;}
"is" {printf("%s\n",yytext); return TOK_IS;}
"Main" {printf("%s\n",yytext); return TOK_MAIN;}
"Put_Line" {printf("%s\n",yytext); return TOK_PUTLINE;}
"Get_Line" {printf("%s\n",yytext); return TOK_GETLINE;}
"loop" {printf("%s\n",yytext); return TOK_LOOP;}

"String" {printf("%s\n",yytext); return TOK_STRING;}
"Natural" {printf("%s\n",yytext); return TOK_INTEGER;}
"Integer" {printf("%s\n",yytext); return TOK_INTEGER;}
"Boolean" {printf("%s\n",yytext); return TOK_BOOL;}


"not" {printf("%s\n",yytext); return TOK_NOT;}
"or" {printf("%s\n",yytext); return TOK_OR;}
"and" {printf("%s\n",yytext); return TOK_AND;}
"if" {printf("%s\n",yytext); return TOK_IF;}
"while" {printf("%s\n",yytext); return TOK_WHILE;}
"else" {printf("%s\n",yytext); return TOK_ELSE;}
"elsif" {printf("%s\n",yytext); return TOK_ELSEIF;}
"then" {printf("%s\n",yytext); return TOK_THEN;}
"xor" {printf("%s\n",yytext); return TOK_XOR;}
"True" {printf("%s\n",yytext); return TOK_TRUE;}
"False" {printf("%s\n",yytext); return TOK_FALSE;}



"=" {printf("=\n"); return TOK_EQ;}
"/=" {printf("/=\n"); return TOK_NOTEQ;}
":=" {printf(":=\n"); return TOK_ASSIGN;}
"<=" {printf("<=\n"); return TOK_LESSEQ;}
">=" {printf(">=\n"); return TOK_GREATEREQ;}
"<" {printf("<\n"); return TOK_LESS;}
">" {printf(">\n"); return TOK_GREATER;}

"+" {printf("+\n"); return TOK_OP_ADD;}
"-" {printf("-\n"); return TOK_OP_MINUS;}
"*" {printf("*\n"); return TOK_OP_MULT;}
"/" {printf("/\n"); return TOK_OP_DIV;}
":" {printf(":\n"); return TOK_COLON;}
"(" {printf("(\n"); return TOK_LP;}
")" {printf(")\n"); return TOK_RP;}
";" {printf(";\n"); return TOK_END_STATEMENT;}
"," {printf(",\n"); return TOK_COMMA;}


{digit}+ {printf("%s\n",yytext); yylval.num = (double)atoi(yytext); return TOK_NUM;}
{digit}+"."{digit}+ {printf("%s\n",yytext); yylval.num = atof(yytext); return TOK_NUM;}
{alpha}({alpha}|{digit})* {printf("%s\n",yytext); yylval.str = strdup(yytext);return TOK_ID; }
\"(\\.|[^"\\])*\"  {printf("%s\n",yytext);yylval.str = strdup(yytext);return TOK_STRLITERAL;}

<<EOF>> {return EOF;}


