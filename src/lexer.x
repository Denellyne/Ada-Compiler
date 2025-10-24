%{
#include "parser.tab.h"

%}
%option noyywrap

digit   [0-9]
alpha [_a-zA-Z]


/* completar: declarações */

%%

[ \t\n\r]+

{digit}+ {printf("%s\n",yytext); yylval.num = (double)atoi(yytext); return TOK_NUM;}
{digit}+"."{digit}+ {printf("%s\n",yytext); yylval.num = atof(yytext); return TOK_NUM;}

{alpha}({alpha}|{digit})* {printf("%s\n",yytext); yylval.str = strdup(yytext);return TOK_ID; }

"=" {printf("%s\n",yytext); return TOK_ASSIGN;}
"++" {printf("%s\n",yytext); return TOK_INCR;}
"+" {printf("%s\n",yytext); return TOK_OP_ADD;}
"-" {printf("%s\n",yytext); return TOK_OP_MINUS;}
"*" {printf("%s\n",yytext); return TOK_OP_MULT;}
"/" {printf("%s\n",yytext); return TOK_OP_DIV;}

"(" {printf("(\n"); return TOK_LP;}
")" {printf(")\n"); return TOK_RP;}
";" {printf(";\n"); return TOK_END_STATEMENT;}


<<EOF>> {return EOF;}


