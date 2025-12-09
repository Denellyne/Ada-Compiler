#include "codeGen.h"
#include <stdio.h>

int printConditional(instrList *ir, FILE *file);
int printAssignment(instrList *ir, FILE *file);
int printLoop(instrList *ir, FILE *file);
int printJump(instrList *ir, FILE *file);
int printDataSection(Table tbl, Stm varDecl, FILE *file, stringLiterals *strs);
int printHeader(FILE *file);
int printMain(FILE *file, instrList *instrs);
int printInstr(FILE *file, instrList *instrs);
int printPowFunction(FILE *file);
int printPutLineFunction(FILE *file);
int printPutNumFunction(FILE *file);
int printCallFunction(FILE *file, char *functionName);
int printStaticString(FILE *file, stringLiterals *strs);

int generateASM(char *fileName, Table tbl, Stm varDecl, instrList *ir,
                stringLiterals *strs) {
  char *outputFile;
  asprintf(&outputFile, "%s.bin", fileName);
  if (!outputFile) {
    fprintf(stderr, "Unable to generate file name for output\n");
    return 0;
  }

  FILE *out = fopen(outputFile, "w");
  if (!out) {
    fprintf(stderr, "File pointer to output binary is null\n");
    return 0;
  }

  if (!printDataSection(tbl, varDecl, out, strs)) {
    fprintf(stderr, "Unable to print .data section to file\n");
    fclose(out);
    return 0;
  }
  if (!printHeader(out)) {
    fprintf(stderr, "Unable to print header section to file\n");
    fclose(out);
    return 0;
  }
  if (!printPowFunction(out)) {
    fprintf(stderr, "Unable to print pow function to file\n");
    fclose(out);
    return 0;
  }
  if (!printPutLineFunction(out)) {
    fprintf(stderr, "Unable to print pow function to file\n");
    fclose(out);
    return 0;
  }
  if (!printPutNumFunction(out)) {
    fprintf(stderr, "Unable to print pow function to file\n");
    fclose(out);
    return 0;
  }
  if (!printMain(out, ir)) {
    fprintf(stderr, "Unable to print main function to file\n");
    fclose(out);
    return 0;
  }
  fclose(out);
  return 1;
}

int printTableVariables(Table tbl, Stm varDecl, FILE *file) {
  if (!tbl || !varDecl)
    return 1;
  switch (varDecl->compound.fst->assign.type) {

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
  }

  return printTableVariables(tbl, varDecl->compound.snd, file);
}
int printDataSection(Table tbl, Stm varDecl, FILE *file, stringLiterals *strs) {
  if (fprintf(file, "\t.data\n") < 0)
    return 0;

  if (!printStaticString(file, strs)) {
    fprintf(stderr, "Unable to print static strings\n");
    return 0;
  }
  return 1;
}
int printHeader(FILE *file) {
  char *header = "_heap_:\t.space 10000\n       \t.text\n       "
                 "       \tla $gp, _heap_\n      \tjal main\n_stop_:\n   "
                 "    \tli $2,10\n       \tsyscall\n";

  if (fprintf(file, "%s\n", header) < 0)
    return 0;
  return 1;
}
int printMain(FILE *file, instrList *instrs) {

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
int printInstr(FILE *file, instrList *instrs) {

  instrList *current = instrs;
  while (current != NULL) {
    switch (current->instr.opcode) {
    case POWER:
      if (fprintf(file, "move $a0,$%s\n", current->instr.arg2) < 0)
        return 0;
      if (fprintf(file, "move $a1,$%s\n", current->instr.arg3) < 0)
        return 0;
      printCallFunction(file, "pow");
      if (fprintf(file, "move $%s,$v0\n", current->instr.arg1) < 0)
        return 0;
      break;
    case POWERI:
      if (fprintf(file, "move $a0,$%s\n", current->instr.arg2) < 0)
        return 0;
      if (fprintf(file, "li $a1,%d\n", current->instr.num) < 0)
        return 0;
      printCallFunction(file, "pow");
      if (fprintf(file, "move $%s,$v0\n", current->instr.arg1) < 0)
        return 0;
      break;
    case XR:
      if (fprintf(file, "xor $%s,$%s, $%s\n", current->instr.arg1,
                  current->instr.arg2, current->instr.arg3) < 0)
        return 0;
      break;
    case XRI:
      if (fprintf(file, "xori $%s,$%s, %d\n", current->instr.arg1,
                  current->instr.arg2, current->instr.num) < 0)
        return 0;
      break;
    case SUB:
      if (fprintf(file, "subu $%s,$%s, $%s\n", current->instr.arg1,
                  current->instr.arg2, current->instr.arg3) < 0)
        return 0;
      break;
    case SUBI:
      if (fprintf(file, "addiu $%s,$%s, -%d\n", current->instr.arg1,
                  current->instr.arg2, current->instr.num) < 0)
        return 0;
      break;
    case MULTI:
      if (fprintf(file, "mul $%s, $%s, %d\n", current->instr.arg1,
                  current->instr.arg2, current->instr.num) < 0)
        return 0;
      break;
    case DIVIDEI:
      if (fprintf(file, "div $%s,$%s, %d\n", current->instr.arg1,
                  current->instr.arg2, current->instr.num) < 0)
        return 0;
      break;
    case MULT:
      if (fprintf(file, "mul $%s, $%s, $%s\n", current->instr.arg1,
                  current->instr.arg2, current->instr.arg3) < 0)
        return 0;
      break;
    case DIVIDE:
      if (fprintf(file, "div $%s,$%s, $%s\n", current->instr.arg1,
                  current->instr.arg2, current->instr.arg3) < 0)
        return 0;
      break;
    case ADD:
      if (fprintf(file, "addu $%s,$%s, $%s\n", current->instr.arg1,
                  current->instr.arg2, current->instr.arg3) < 0)
        return 0;
      break;
    case ADDI:
      if (fprintf(file, "addiu $%s,$%s, %d\n", current->instr.arg1,
                  current->instr.arg2, current->instr.num) < 0)
        return 0;
      break;
    case LOADADRESS:
      if (fprintf(file, "la $%s, %s\n", current->instr.arg1,
                  current->instr.arg2) < 0)
        return 0;
      break;
    case MOVE:
      if (fprintf(file, "move $%s, $%s\n", current->instr.arg1,
                  current->instr.arg2) < 0)
        return 0;
      break;
    case NEG:
      if (fprintf(file, "sltiu $%s, $%s,1\n", current->instr.arg1,
                  current->instr.arg1) < 0)
        return 0;
      break;
    case CALL:

      if (printCallFunction(file, current->instr.arg1) == 0)
        return 0;
      break;
    case MOVEI:
      if (fprintf(file, "li $%s, %d\n", current->instr.arg1,
                  current->instr.num) < 0)
        return 0;
      break;
    case BNEZ:
      if (fprintf(file, "bnez $%s, %s\n", current->instr.arg1,
                  current->instr.arg3) < 0)
        return 0;
      break;
    case EQUALSI:
      if (fprintf(file, "beq $%s, $%d, %s\n", current->instr.arg1,
                  current->instr.num, current->instr.arg3) < 0)
        return 0;
      break;

    case NOTEQUALSI:
      if (fprintf(file, "bne $%s, $%d, %s\n", current->instr.arg1,
                  current->instr.num, current->instr.arg3) < 0)
        return 0;
      break;

    case LESSERI:
      if (fprintf(file, "blt $%s, $%d, %s\n", current->instr.arg1,
                  current->instr.num, current->instr.arg3) < 0)
        return 0;
      break;

    case GREATERI:
      if (fprintf(file, "bgt $%s, $%d, %s\n", current->instr.arg1,
                  current->instr.num, current->instr.arg3) < 0)
        return 0;
      break;

    case LESSEREQI:
      if (fprintf(file, "ble $%s, $%d, %s\n", current->instr.arg1,
                  current->instr.num, current->instr.arg3) < 0)
        return 0;
      break;

    case GREATEREQI:
      if (fprintf(file, "bge $%s, $%d, %s\n", current->instr.arg1,
                  current->instr.num, current->instr.arg3) < 0)
        return 0;
      break;
    case EQUALS:
      if (fprintf(file, "beq $%s,$%s,%s\n", current->instr.arg1,
                  current->instr.arg2, current->instr.arg3) < 0)
        return 0;
      break;

    case NOTEQUALS:
      if (fprintf(file, "bne $%s,$%s,%s\n", current->instr.arg1,
                  current->instr.arg2, current->instr.arg3) < 0)
        return 0;
      break;

    case LESSER:
      if (fprintf(file, "blt $%s,$%s,%s\n", current->instr.arg1,
                  current->instr.arg2, current->instr.arg3) < 0)
        return 0;
      break;

    case GREATER:
      if (fprintf(file, "bgt $%s,$%s,%s\n", current->instr.arg1,
                  current->instr.arg2, current->instr.arg3) < 0)
        return 0;
      break;

    case LESSEREQ:
      if (fprintf(file, "ble $%s,$%s,%s\n", current->instr.arg1,
                  current->instr.arg2, current->instr.arg3) < 0)
        return 0;
      break;

    case GREATEREQ:
      if (fprintf(file, "bge $%s,$%s,%s\n", current->instr.arg1,
                  current->instr.arg2, current->instr.arg3) < 0)
        return 0;
      break;
    case OP: {
      char *op_str;
      switch (current->instr.binop) {
      case AND:
        op_str = "and";
        break;
      case OR:
        op_str = "or";
        break;
      case EQ:
        op_str = "=";
        break;
      case NEQ:
        op_str = "/=";
        break;
      case LT:
        op_str = "<";
        break;
      case GT:
        op_str = ">";
        break;
      case LE:
        op_str = "<=";
        break;
      case GE:
        op_str = ">=";
        break;
      default:
        op_str = "?";
      }
      // printf("%s := %s %s %s\n", current->instr.arg1, current->instr.arg2,
      //        op_str, current->instr.arg3);
      break;
    }
    case LABEL:
      if (fprintf(file, "%s:\n", current->instr.arg1) < 0)
        return 0;
      break;
    case JUMP:
      if (fprintf(file, "j %s\n", current->instr.arg1) < 0)
        return 0;
      break;
    case COND: {
      switch (current->instr.binop) {

      case EQ:
        if (fprintf(file, "bne $%s, $%s, %s\n", current->instr.arg1,
                    current->instr.arg2, current->instr.arg4) < 0)
          return 0;
        break;
      case NEQ:
        if (fprintf(file, "beq $%s, $%s, %s\n", current->instr.arg1,
                    current->instr.arg2, current->instr.arg3) < 0)
          return 0;
        break;
      case LT:
        if (fprintf(file, "bge $%s, $%s, %s\n", current->instr.arg1,
                    current->instr.arg2, current->instr.arg4) < 0)
          return 0;
        break;
      case GT:
        if (fprintf(file, "ble $%s, $%s, %s\n", current->instr.arg1,
                    current->instr.arg2, current->instr.arg4) < 0)
          return 0;
        break;
      case LE:
        if (fprintf(file, "bgt $%s, $%s, %s\n", current->instr.arg1,
                    current->instr.arg2, current->instr.arg4) < 0)
          return 0;
        break;
      case GE:
        if (fprintf(file, "blt $%s, $%s, %s\n", current->instr.arg1,
                    current->instr.arg2, current->instr.arg4) < 0)
          return 0;
        break;
      }
      break;
    }
    }
    current = current->next;
  }
  return 1;
}

int printPowFunction(FILE *file) {
  char *powASMString = "pow:\n\
addiu $sp, $sp, -8 \n\
sw $fp, 0($sp)\n\
sw $ra, 4($sp)\n\
move $fp, $sp\n\n\
beq $a1,$zero,pow_0\n\
beq $a1,1,pow_ret\n\
move $v0,$a0\n\
move $t0,$a1\n\n\
pow_start:\n\
mul $v0,$v0,$a0\n\
addiu $t0,$t0,-1\n\
beq $t0,1,pow_ret\n\
j pow_start\n\
pow_0:\n\n\
li $v0,1\n\
pow_ret:\n\n\
move $sp, $fp\n\
lw $ra, 4($sp)\n\
lw $fp, 0($sp)\n\
addiu $sp, $sp, 8\n\
jr $ra\n";

  if (fprintf(file, "\n%s\n", powASMString) < 0)
    return 0;
  return 1;
}
int printCallFunction(FILE *file, char *functionName) {
  if (fprintf(file,
              "\naddiu $sp, $sp, -4\nsw    $ra, 0($sp)\njal %s\nlw    $ra, "
              "0($sp)\naddiu $sp, $sp, 4\n",
              functionName) < 0)
    return 0;
  return 1;
}
int printStaticString(FILE *file, stringLiterals *strs) {
  if (!strs)
    return 1;
  if (fprintf(file, "%s: \t.asciiz %s\n", strs->id, strs->str) < 0)
    return 0;
  return printStaticString(file, strs->next);
}
int printPutLineFunction(FILE *file) {

  char *putLine = "Put_Line:\n\
addiu $sp, $sp, -8 \n\
sw $fp, 0($sp)\n\
sw $ra, 4($sp)\n\
move $fp, $sp\n\
\n\
li $v0, 4\n\
syscall\n\
\n\
move $sp, $fp\n\
lw $ra, 4($sp)\n\
lw $fp, 0($sp)\n\
addiu $sp, $sp, 8\n\
jr $ra\n";

  if (fprintf(file, "\n%s\n", putLine) < 0)
    return 0;
  return 1;
}

int printPutNumFunction(FILE *file) {

  char *putLine = "Put_Num:\n\
addiu $sp, $sp, -8 \n\
sw $fp, 0($sp)\n\
sw $ra, 4($sp)\n\
move $fp, $sp\n\
\n\
li $v0, 1\n\
syscall\n\
\n\
move $sp, $fp\n\
lw $ra, 4($sp)\n\
lw $fp, 0($sp)\n\
addiu $sp, $sp, 8\n\
jr $ra\n";

  if (fprintf(file, "\n%s\n", putLine) < 0)
    return 0;
  return 1;
}
