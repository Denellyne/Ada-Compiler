#include "codeGen.h"
#include <stdio.h>

int printConditional(InstrList *ir, FILE *file);
int printAssignment(InstrList *ir, FILE *file);
int printLoop(InstrList *ir, FILE *file);
int printJump(InstrList *ir, FILE *file);
int printDataSection(Table tbl, Stm varDecl, FILE *file);
int printHeader(FILE *file);
int printMain(FILE *file, InstrList *instrs);
int printInstr(FILE *file, InstrList *instrs);

void codeGen(Table tbl, Stm varDecl, InstrList *ir) {
  FILE *out = fopen("out.bin", "w");
  if (!out) {
    fprintf(stderr, "File pointer to output binary is null\n");
    return;
  }

  if (!printDataSection(tbl, varDecl, out)) {
    fprintf(stderr, "Unable to print .data section to file\n");
    fclose(out);
    return;
  }
  if (!printHeader(out)) {
    fprintf(stderr, "Unable to print header section to file\n");
    fclose(out);
    return;
  }
  if (!printMain(out, ir)) {
    fprintf(stderr, "Unable to print main function to file\n");
    fclose(out);
    return;
  }
  fclose(out);
}

int printTableVariables(Table tbl, Stm varDecl, FILE *file) {
  if (!tbl || !varDecl)
    return 1;
  switch (varDecl->compound.fst->assign.type) {

  case NUM:
    if (lookup(tbl, varDecl->compound.fst->assign.ident) == NULL) {
      fprintf(stderr,
              "Table doesn't have symbol %s present on variable declaration\n",
              varDecl->compound.fst->assign.ident);
      return 0;
    }
    if (fprintf(file, "%s: \t.word %d\n", varDecl->compound.fst->assign.ident,
                (int)varDecl->compound.fst->assign.expr->val) < 0)
      return 0;
    break;
  case BOOL:
    if (lookup(tbl, varDecl->compound.fst->assign.ident) == NULL) {
      fprintf(stderr,
              "Table doesn't have symbol %s present on variable declaration\n",
              varDecl->compound.fst->assign.ident);
      return 0;
    }
    if (varDecl->compound.fst->assign.expr->bool_val == 1) {
      if (fprintf(file, "%s: \t.word 1\n",
                  varDecl->compound.fst->assign.ident) < 0)
        return 0;
    } else {
      if (fprintf(file, "%s: \t.word 0\n",
                  varDecl->compound.fst->assign.ident) < 0)
        return 0;
    }
    break;
  case STRLITERAL:
    if (lookup(tbl, varDecl->compound.fst->assign.ident) == NULL) {
      fprintf(stderr,
              "Table doesn't have symbol %s present on variable declaration\n",
              varDecl->compound.fst->assign.ident);
      return 0;
    }
    if (fprintf(file, "%s: \t.asciiz %s\n", varDecl->compound.fst->assign.ident,
                varDecl->compound.fst->assign.expr->str) < 0)
      return 0;
    break;
  case ID:
    break;
  }

  return printTableVariables(tbl, varDecl->compound.snd, file);
}
int printDataSection(Table tbl, Stm varDecl, FILE *file) {
  if (fprintf(file, "\t.data\n") < 0)
    return 0;

  return printTableVariables(tbl, varDecl, file);
}
int printHeader(FILE *file) {
  char *header = "_heap_:\t.space 10000\n       \t.text\n       \t.global "
                 "main\n       \tla $gp, _heap_\n      \tjal main\n_stop_:\n   "
                 "    \tli $2,10\n       \tsyscall\n";

  if (fprintf(file, "%s\n", header) < 0)
    return 0;
  return 1;
}
int printMain(FILE *file, InstrList *instrs) {

  if (fprintf(file, "main:\n") < 0)
    return 0;
  if (!printInstr(file, instrs)) {

    fprintf(stderr,
            "Unable to print assembly for the Instruction List given\n");
    return 0;
  }
  if (fprintf(file, "jr $ra\n") < 0)
    return 0;
  return 1;
}
int printInstr(FILE *file, InstrList *instrs) {}
