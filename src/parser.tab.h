/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_PARSER_TAB_H_INCLUDED
# define YY_YY_PARSER_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif
/* "%code requires" blocks.  */
#line 8 "Parser/parser.y"


#include "Ast/ast.h"

#line 54 "parser.tab.h"

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    TOK_NUM = 258,                 /* TOK_NUM  */
    TOK_ID = 259,                  /* TOK_ID  */
    TOK_ASSIGN = 260,              /* TOK_ASSIGN  */
    TOK_INCR = 261,                /* TOK_INCR  */
    TOK_OP_ADD = 262,              /* TOK_OP_ADD  */
    TOK_OP_MINUS = 263,            /* TOK_OP_MINUS  */
    TOK_OP_MULT = 264,             /* TOK_OP_MULT  */
    TOK_OP_DIV = 265,              /* TOK_OP_DIV  */
    TOK_END_STATEMENT = 266,       /* TOK_END_STATEMENT  */
    TOK_LP = 267,                  /* TOK_LP  */
    TOK_RP = 268,                  /* TOK_RP  */
    TOK_BEGIN = 269,               /* TOK_BEGIN  */
    TOK_END = 270,                 /* TOK_END  */
    TOK_PROC = 271,                /* TOK_PROC  */
    TOK_IS = 272,                  /* TOK_IS  */
    TOK_MAIN = 273,                /* TOK_MAIN  */
    TOK_PUTLINE = 274              /* TOK_PUTLINE  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 12 "Parser/parser.y"

  Stm stm;
  Exp exp;
  double num;
  char* str;

#line 97 "parser.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_PARSER_TAB_H_INCLUDED  */
