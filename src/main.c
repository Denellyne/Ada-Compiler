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

#include "parser.tab.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  freopen(argv[1], "r", stdin);
  yyparse();
  return EXIT_SUCCESS;
}
