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
  tbl = addEntry(tbl, "Put_Line", TBL_FUNCTION, 1);
  tbl = addEntry(tbl, "Put_Num", TBL_FUNCTION, 1);
  tbl = addEntry(tbl, "Get_Line", TBL_FUNCTION, 2);
  freopen(argv[1], "r", stdin);
  int res = yyparse(&prog);
  if (res != 0) {
    return EXIT_FAILURE;
  }
  tbl = addVariableDeclarations(tbl, prog->varDec);
  if (!tbl)
    return EXIT_FAILURE;
  printTable(tbl);

  stringLiterals *strs = NULL;
  instrList *instrs = generateIR(prog, &strs, tbl);
  if (!instrs) {
    fprintf(stderr, "Unable to generate the IR for the source code given\n");
    return EXIT_FAILURE;
  }
  printInstructions(instrs);
  if (!generateASM(tbl, prog->varDec, instrs, strs)) {
    freeInstructions(&instrs);
    freeStrings(&strs);
    fprintf(stderr,
            "Unable to generate the assembly for the source code given\n");
    return EXIT_FAILURE;
  }
  freeInstructions(&instrs);
  freeStrings(&strs);
  return EXIT_SUCCESS;
}
