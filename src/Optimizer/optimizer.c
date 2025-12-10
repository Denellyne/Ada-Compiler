#include "optimizer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int optimizeMoveI(instrList *ir, stringLiterals **strs,
                  floatLiterals **floats) {
  if (!ir->next)
    return 1;

  switch (ir->next->instr.opcode) {
  case ADD:
  case SUB:
  case MULT:
  case DIVIDE:
  case POWER:
  case ADDI:
  case SUBI:
  case MULTI:
  case DIVIDEI:
  case POWERI: {
    char *t0 = ir->instr.arg1;
    char *t1 = ir->instr.arg2;
    char *t2 = ir->next->instr.arg1;
    char *t3 = ir->next->instr.arg2;
    if (!strcmp(t0, t3)) {
      ir->instr.arg1 = t2;
      ir->next->instr.arg2 = t2;
    }

  } break;
  case MOVE: {
    char *t0 = ir->instr.arg1;
    char *t1 = ir->instr.arg2;
    char *t2 = ir->next->instr.arg1;
    char *t3 = ir->next->instr.arg2;
    if (!strcmp(t0, t3)) {
      ir->instr.arg1 = t2;
      instrList *nextIr = ir->next->next;
      free(ir->next);
      ir->next = nextIr;
    }
  } break;
  default:
    break;
  }
  return 1;
}

int optimizeArith(instrList *ir, stringLiterals **strs, floatLiterals **floats,
                  int *changed) {
  if (!ir->next)
    return 1;

  switch (ir->next->instr.opcode) {
  case ADD:
  case SUB:
  case MULT:
  case DIVIDE:
  case POWER:
  case ADDI:
  case SUBI:
  case MULTI:
  case DIVIDEI:
  case POWERI: {
    char *t0 = ir->instr.arg1;
    char *t1 = ir->instr.arg2;
    char *t2 = ir->next->instr.arg1;
    char *t3 = ir->next->instr.arg2;
    if (!strcmp(t0, t3) && strcmp(t0, t2)) {
      ir->instr.arg1 = t2;
      ir->next->instr.arg2 = t2;
      *changed = 1;
    }

  } break;
  case MOVE: {
    char *t0 = ir->instr.arg1;
    char *t1 = ir->instr.arg2;
    char *t2 = ir->next->instr.arg1;
    char *t3 = ir->next->instr.arg2;
    if (!strcmp(t0, t3)) {
      ir->instr.arg1 = t2;
      instrList *nextIr = ir->next->next;
      free(ir->next);
      ir->next = nextIr;
      *changed = 1;
    }
  } break;
  default:
    break;
  }
  return 1;
}

instrList *optimizeIR(instrList *ir, stringLiterals **strs,
                      floatLiterals **floats, int *changed) {
  if (!ir) {
    fprintf(stderr, "IR passed to optimizer was null\n");
    return NULL;
  }
  *changed = 0;
  printf("Optimizing IR\n");

  instrList *head = ir;
  // printInstructions(ir);
  while (head) {
    switch (head->instr.opcode) {
    case ADDI:
    case SUBI:
    case MULTI:
    case DIVIDEI:
    case POWERI:
    case ADD:
    case SUB:
    case MULT:
    case DIVIDE:
    case POWER:
      if (!optimizeArith(head, strs, floats, changed)) {
        fprintf(stderr, "Optimizer error while optimizing arith\n");
        return NULL;
      }
      break;
    case MOVEI:
      break;
      if (!optimizeMoveI(head, strs, floats)) {
        fprintf(stderr, "Optimizer error while optimizing arith\n");
        return NULL;
      }
    default:
      break;
    }
    head = head->next;
  }
  // printInstructions(ir);
  printf("Optimizer Finished\n");

  return ir;
}
