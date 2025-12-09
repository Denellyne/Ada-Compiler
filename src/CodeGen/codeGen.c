#include "codeGen.h"
#include <stdio.h>

int printConditional(instrList *ir, FILE *file);
int printAssignment(instrList *ir, FILE *file);
int printLoop(instrList *ir, FILE *file);
int printJump(instrList *ir, FILE *file);
int printDataSection(Table tbl, Stm varDecl, FILE *file, stringLiterals *strs,
                     floatLiterals *floats);
int printHeader(FILE *file);
int printMain(FILE *file, instrList *instrs);
int printInstr(FILE *file, instrList *instrs);
int printPowFunction(FILE *file);
int printPutLineFunction(FILE *file);
int printGetLineFunction(FILE *file);
int printPutNumFunction(FILE *file);
int printCallFunction(FILE *file, char *functionName);
int printStaticString(FILE *file, stringLiterals *strs);
int printStaticFloats(FILE *file, floatLiterals *floats);
int printSaveRegisters(FILE *file, int regs);
int printLoadRegisters(FILE *file, int regs);

int generateASM(char *fileName, Table tbl, Stm varDecl, instrList *ir,
                stringLiterals *strs, floatLiterals *floats) {
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

  if (!printDataSection(tbl, varDecl, out, strs, floats)) {
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
  if (!printGetLineFunction(out)) {
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
int printDataSection(Table tbl, Stm varDecl, FILE *file, stringLiterals *strs,
                     floatLiterals *floats) {
  if (fprintf(file, "\t.data\n\tbuffer: .space 64\n") < 0)
    return 0;

  if (!printStaticString(file, strs)) {
    fprintf(stderr, "Unable to print static strings\n");
    return 0;
  }
  if (!printStaticFloats(file, floats)) {
    fprintf(stderr, "Unable to print constant floats\n");
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
    case LOADREGISTERS:
      if (!printLoadRegisters(file, current->instr.num)) {
        fprintf(stderr, "Unable to print ASM to load registers\n");
        return 0;
      }
      break;
    case SAVEREGISTERS:
      if (!printSaveRegisters(file, current->instr.num)) {
        fprintf(stderr, "Unable to print ASM to save registers\n");
        return 0;
      }
      break;
    case POWERF:
      if (fprintf(file, "mov.d $f12,$%s\n", current->instr.arg2) < 0)
        return 0;
      if (fprintf(file, "move $a1,$%s\n", current->instr.arg3) < 0)
        return 0;
      if (fprintf(file, "li $a2,1\n") < 0)
        return 0;
      if (!printCallFunction(file, "pow")) {
        fprintf(stderr, "Unable to print function for the Power operator\n");
        return 0;
      }
      if (fprintf(file, "mov.d $%s,$f30\n", current->instr.arg1) < 0)
        return 0;
      break;
    case POWER:
      if (fprintf(file, "move $a0,$%s\n", current->instr.arg2) < 0)
        return 0;
      if (fprintf(file, "move $a1,$%s\n", current->instr.arg3) < 0)
        return 0;
      if (fprintf(file, "li $a2,0\n") < 0)
        return 0;
      if (!printCallFunction(file, "pow")) {
        fprintf(stderr, "Unable to print function for the Power operator\n");
        return 0;
      }
      if (fprintf(file, "move $%s,$v0\n", current->instr.arg1) < 0)
        return 0;
      break;
    case POWERI:
      if (fprintf(file, "move $a0,$%s\n", current->instr.arg2) < 0)
        return 0;
      if (fprintf(file, "li $a1,%d\n", current->instr.num) < 0)
        return 0;
      if (!printCallFunction(file, "pow")) {
        fprintf(stderr, "Unable to print function for the Power operator\n");
        return 0;
      }
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
    case SUBF:
      if (fprintf(file, "sub.d $%s,$%s, $%s\n", current->instr.arg1,
                  current->instr.arg2, current->instr.arg3) < 0)
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
    case MULTF:
      if (fprintf(file, "mul.d $%s, $%s, $%s\n", current->instr.arg1,
                  current->instr.arg2, current->instr.arg3) < 0)
        return 0;
      break;
    case MULT:
      if (fprintf(file, "mul $%s, $%s, $%s\n", current->instr.arg1,
                  current->instr.arg2, current->instr.arg3) < 0)
        return 0;
      break;
    case DIVIDEF:
      if (fprintf(file, "div.f $%s,$%s, $%s\n", current->instr.arg1,
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
    case ADDF:
      if (fprintf(file, "add.d $%s,$%s, $%s\n", current->instr.arg1,
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
    case MOVEFI:
      if (fprintf(file, "ldc1 $%s, %s\n", current->instr.arg1,
                  current->instr.arg2) < 0)
        return 0;
      break;
    case MOVEF:
      if (fprintf(file, "mov.d $%s, $%s\n", current->instr.arg1,
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
int printLoadRegisters(FILE *file, int regs) {
  if (regs == 0)
    return 1;
  char *freeStack = "";
  int size = regs * 4;
  asprintf(&freeStack, "addi $sp,$sp,%d\n", size);
  if (!freeStack) {
    fprintf(stderr, "Unable to alloc memory for registers\n");
    return 0;
  }

  char *loadRegistersString = "";
  for (int i = 0; i < regs; i++) {
    asprintf(&loadRegistersString, "%s\nlw $%s,%d($sp)", loadRegistersString,
             temps[i], (i + 1) * 4);
    if (!loadRegistersString) {
      fprintf(stderr, "Unable to write to string to save the registers\n");
      return 0;
    }
  }

  if (fprintf(file, "%s\n%s", loadRegistersString, freeStack) < 0) {
    fprintf(stderr, "Unable to write ASM to save registers to file\n");
    return 0;
  }
  return 1;
}
int printSaveRegisters(FILE *file, int regs) {
  if (regs == 0)
    return 1;
  char *allocStack = "";
  int size = regs * 4;
  asprintf(&allocStack, "addi $sp,$sp,-%d\n", size);
  if (!allocStack) {
    fprintf(stderr, "Unable to alloc memory for registers\n");
    return 0;
  }

  char *saveRegistersString = "";
  for (int i = 0; i < regs; i++) {
    asprintf(&saveRegistersString, "%s\nsw $%s,%d($sp)", saveRegistersString,
             temps[i], ((i + 1) * 4));
    if (!saveRegistersString) {
      fprintf(stderr, "Unable to write to string to save the registers\n");
      return 0;
    }
  }

  if (fprintf(file, "%s\n%s", allocStack, saveRegistersString) < 0) {
    fprintf(stderr, "Unable to write ASM to save registers to file\n");
    return 0;
  }
  return 1;
}

int printPowFunction(FILE *file) {
  char *powASMString = "pow:\n\
         addiu $sp, $sp, -8 \n\
         sw $fp, 0($sp)\n\
         sw $ra, 4($sp)\n\
         move $fp, $sp\n\
         bnez $a2,powFloat\n\
powNum:\n\
         beq $a1,$zero,powNum_0\n\
         beq $a1,1,pow_ret\n\
         move $v0,$a0\n\
         move $t0,$a1\n\
\n\
powNum_start:\n\
         mul $v0,$v0,$a0\n\
         addiu $t0,$t0,-1\n\
         beq $t0,1,pow_ret\n\
         j powNum_start\n\
powNum_0:\n\
\n\
         li $v0,1\n\
       	 j pow_ret\n\
powFloat:\n\
	 beq $a1,$zero,powFloat_0\n\
         beq $a1,1,pow_ret\n\
         mov.d $f30,$f12\n\
         move $t0,$a1\n\
powFloat_Start:\n\
         mul.d $f30,$f30,$f12\n\
         addiu $t0,$t0,-1\n\
         beq $t0,1,pow_ret\n\
         j powFloat_Start\n\
powFloat_0:\n\
\n\
         mtc1.d $zero,$f30\n\
       	 j pow_ret\n\
pow_ret:\n\
\n\
         move $sp, $fp\n\
         lw $ra, 4($sp)\n\
         lw $fp, 0($sp)\n\
         addiu $sp, $sp, 8\n\
         jr $ra\n\
";

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
int printStaticFloats(FILE *file, floatLiterals *floats) {
  if (!floats)
    return 1;
  if (fprintf(file, "%s: \t.double %f\n", floats->id, floats->val) < 0)
    return 0;
  return printStaticFloats(file, floats->next);
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
int printGetLineFunction(FILE *file) {

  char *putLine = "Get_Line:\n\
         addiu $sp, $sp, -8 \n\
         sw $fp, 0($sp)\n\
         sw $ra, 4($sp)\n\
         move $fp, $sp\n\
\
         move $a0,$t0\n\
         la $a0,askInputStr\n\
         li $v0,4\n\
         syscall\n\
\
         li $v0,8 #take in input\n\
         la $a0, buffer #load byte space into address\n\
         li $a1, 64 # allot the byte space for string\n\
         move $t1,$a0 #save string to t1\n\
         syscall\n\
         move $v0,$t1 #save string to v0\n\
\
\n\
getLength:\n\
    	 lb   $a1, 0($v0)\n\
    	 beq  $a1, $zero, getLengthEnd\n\
\n\
    	 addi $v0, $v0 1\n\
   	 j getLength\n\
\n\
getLengthEnd:\n\
\n\
	 la $a1, ($a0)\n\
	 sub $a1, $v0, $a1\n\
	 addiu $a1,$a1,-1\n\
	 move $v1,$a0\n\
	 li $v0,9\n\
	 li $a0,8\n\
	 syscall\n\
	 move $a0,$v0\n\
	 \n\
copyStr:\n\
	lbu $a2, 0($v1)\n\
	sb $a2,0($v0)\n\
	addi $v1,$v1,1\n\
	addi $v0,$v0,1\n\
	bne $a2, $zero,copyStr\n\
\n\
copyStrEnd:\n\
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
         bnez $a1,isFloat\n\
isNum:      \n\
         li $v0, 1\n\
         j Put_NumEnd\n\
isFloat:\n\
         li $v0, 3      \n\
         j Put_NumEnd         \n\
Put_NumEnd:\n\
	 syscall\n\
         move $sp, $fp\n\
         lw $ra, 4($sp)\n\
         lw $fp, 0($sp)\n\
         addiu $sp, $sp, 8\n\
         jr $ra\n\
";

  if (fprintf(file, "\n%s\n", putLine) < 0)
    return 0;
  return 1;
}
