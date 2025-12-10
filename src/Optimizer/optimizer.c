#include "optimizer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int convertToImmediate(const int tag) {
  switch (tag) {
  case MOVE:
    return MOVEI;
  case ADD:
    return ADDI;
  case SUB:
    return SUBI;
  case MULT:
    return MULTI;
  case DIVIDE:
    return DIVIDEI;
  default:
    return -1;
  }
}

int optimizeCond(instrList *ir, stringLiterals **strs, floatLiterals **floats) {
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
int optimizeMoveIRecur(instrList *ir, stringLiterals **strs,
                       floatLiterals **floats, char *id, int val,
                       int *changed) {

  switch (ir->instr.opcode) {
  case ADD:
  case SUB:
  case MULT:
  case DIVIDE:
  case POWER: {
    char *t0 = ir->instr.arg1;
    char *t1 = ir->instr.arg2;
    char *t2 = ir->instr.arg3;
    if (!strcmp(t0, id))
      return 1;
    if (!strcmp(t1, id)) {
      ir->instr.arg2 = t2;
      ir->instr.opcode = convertToImmediate(ir->instr.opcode);
      ir->instr.num = val;
      *changed = 1;
      break;
    } else if (!strcmp(t2, id)) {
      ir->instr.opcode = convertToImmediate(ir->instr.opcode);
      ir->instr.num = val;
      *changed = 1;
      break;
    }

  } break;
  case MOVE: {
    char *t0 = ir->instr.arg1;
    if (!strcmp(t0, id))
      return 1;
    char *t1 = ir->instr.arg2;
    if (!strcmp(id, t1)) {
      ir->instr.num = val;
      ir->instr.opcode = convertToImmediate(ir->instr.opcode);
      if (ir->next) {
        instrList *nextIr = ir->next->next;
        free(ir->next);
        ir->next = nextIr;
      }
      *changed = 1;
    }
  } break;
  default:
    break;
  }
  if (!ir->next)
    return 1;
  instrList *next = ir->next;
  int ret = optimizeMoveIRecur(next, strs, floats, id, val, changed);

  if (!ret) {
    fprintf(stderr, "Error while optimizing MOVEI\n");
    return 0;
  }
  return 1;
}
int optimizeMoveI(instrList *ir, stringLiterals **strs, floatLiterals **floats,
                  int *changed) {
  if (!ir->next || !ir->next->next)
    return 1;
  instrList *next = ir->next;
  int ret = optimizeMoveIRecur(next, strs, floats, ir->instr.arg1,
                               ir->instr.num, changed);
  if (!ret) {
    fprintf(stderr, "Error while optimizing MOVEI\n");
    return 0;
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
      if (!optimizeMoveI(head, strs, floats, changed)) {
        fprintf(stderr, "Optimizer error while optimizing arith\n");
        return NULL;
      }
      break;
    case EQUALS:
    case NOTEQUALS:
    case GREATEREQ:
    case GREATER:
    case LESSER:
    case LESSEREQ:
    case BNEZ:
      break;
      if (!optimizeCond(head, strs, floats)) {
        fprintf(stderr, "Optimizer error while optimizing arith\n");
        return NULL;
      }
      break;

    default:
      break;
    }
    head = head->next;
  }
  // printInstructions(ir);
  printf("Optimizer Finished\n");

  return ir;
}
