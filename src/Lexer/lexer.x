%{
#include "parser.tab.h"

%}
%option noyywrap

digit   [0-9]
alpha [_a-zA-Z]


/* completar: declarações */

%%

[ \t\n\r]+



"begin" {printf("%s ",yytext); return TOK_BEGIN;}
"end" {printf("%s ",yytext); return TOK_END;}
"procedure" {printf("%s ",yytext); return TOK_PROC;}
"is" {printf("%s ",yytext); return TOK_IS;}
"Main" {printf("%s ",yytext); return TOK_MAIN;}
"loop" {printf("%s ",yytext); return TOK_LOOP;}

"String" {printf("%s ",yytext); return TOK_STRING;}
"Natural" {printf("%s ",yytext); return TOK_INTEGER;}
"Integer" {printf("%s ",yytext); return TOK_INTEGER;}
"Boolean" {printf("%s ",yytext); return TOK_BOOL;}


"not" {printf("%s ",yytext); return TOK_NOT;}
"or" {printf("%s ",yytext); return TOK_OR;}
"and" {printf("%s ",yytext); return TOK_AND;}
"if" {printf("%s ",yytext); return TOK_IF;}
"while" {printf("%s ",yytext); return TOK_WHILE;}
"else" {printf("%s ",yytext); return TOK_ELSE;}
"elsif" {printf("%s ",yytext); return TOK_ELSEIF;}
"then" {printf("%s ",yytext); return TOK_THEN;}
"xor" {printf("%s ",yytext); return TOK_XOR;}
"True" {printf("%s ",yytext); return TOK_TRUE;}
"False" {printf("%s ",yytext); return TOK_FALSE;}



"=" {printf("= "); return TOK_EQ;}
"/=" {printf("/= "); return TOK_NOTEQ;}
":=" {printf(":= "); return TOK_ASSIGN;}
"<=" {printf("<= "); return TOK_LESSEQ;}
">=" {printf(">= "); return TOK_GREATEREQ;}
"<" {printf("< "); return TOK_LESS;}
">" {printf("> "); return TOK_GREATER;}

"+" {printf("+ "); return TOK_OP_ADD;}
"-" {printf("- "); return TOK_OP_MINUS;}
"*" {printf("* "); return TOK_OP_MULT;}
"**" {printf("** "); return TOK_POW;}
"/" {printf("/ "); return TOK_OP_DIV;}
":" {printf(": "); return TOK_COLON;}
"(" {printf("( "); return TOK_LP;}
")" {printf(") "); return TOK_RP;}
";" {printf("; "); return TOK_END_STATEMENT;}
"," {printf(", "); return TOK_COMMA;}


{digit}+ {printf("%s ",yytext); yylval.num = (double)atoi(yytext); return TOK_NUM;}
{digit}+"."{digit}+ {printf("%s ",yytext); yylval.num = atof(yytext); return TOK_NUM;}
({alpha})({alpha}|{digit})* {printf("%s ",yytext); yylval.str = strdup(yytext);return TOK_ID; }
\"(\\.|[^"\\])*\"  {printf("%s ",yytext);yylval.str = strdup(yytext);return TOK_STRLITERAL;}

<<EOF>> {return EOF;}


