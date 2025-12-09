/*
  Uma calculadora simples em Haskell
  ----------------------------------

  Completar:
  - o analisador lexical no ficheiro lexer.x;
  - o analisador sintático no ficheiro parser.y

  O analisador sintático devolve diretamente o valor da expressão
  (float) sem construir uma árvore sintática.

  Este módulo apenas lê uma linha de input e invoca
  o analisador lexical e sintático em sequência.

*/

#include "Ast/ast.h"
#include "CodeGen/codeGen.h"
#include "IR/ir.h"
#include "Table/table.h"
#include "parser.tab.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  Prog prog = NULL;
  Table tbl = NULL;
  tbl = addEntry(tbl, "Put_Line", TBL_FUNCTION, 1, TBL_STRING);
  if (!tbl) {
    fprintf(stderr, "Unable to add static function to table\n");
    return EXIT_FAILURE;
  }
  tbl = addEntry(tbl, "Put_Num", TBL_FUNCTION, 1, TBL_INT | TBL_BOOL);
  if (!tbl) {
    fprintf(stderr, "Unable to add static function to table\n");
    return EXIT_FAILURE;
  }
  tbl = addEntry(tbl, "Get_Line", TBL_FUNCTION, 2, TBL_STRING | TBL_ID,
                 TBL_INT | TBL_ID);
  if (!tbl) {
    fprintf(stderr, "Unable to add static function to table\n");
    return EXIT_FAILURE;
  }
  freopen(argv[1], "r", stdin);
  int res = yyparse(&prog);
  if (res != 0)
    return EXIT_FAILURE;

  if (!prog)
    return EXIT_FAILURE;

  tbl = addVariableDeclarations(tbl, prog->varDec);
  if (!tbl)
    return EXIT_FAILURE;

  if (!validateAST(tbl, prog->statements))
    return EXIT_FAILURE;

  printTable(tbl);

  stringLiterals *strs = NULL;
  floatLiterals *floats = NULL;
  instrList *instrs = generateIR(prog, &strs, &floats);
  if (!instrs) {
    fprintf(stderr, "Unable to generate the IR for the source code given\n");
    return EXIT_FAILURE;
  }
  printInstructions(instrs);
  if (!generateASM(argv[1], tbl, prog->varDec, instrs, strs, floats)) {
    freeInstructions(&instrs);
    freeStrings(&strs);
    freeFloats(&floats);
    fprintf(stderr,
            "Unable to generate the assembly for the source code given\n");
    return EXIT_FAILURE;
  }
  freeInstructions(&instrs);
  freeStrings(&strs);
  freeFloats(&floats);
  return EXIT_SUCCESS;
}
