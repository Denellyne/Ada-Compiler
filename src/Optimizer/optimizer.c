#include "optimizer.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int reverseCondition(const int tag) {
  switch (tag) {
  case NOTEQUALSI:
    return EQUALSI;
  case EQUALSI:
    return NOTEQUALSI;
  case LESSEREQI:
    return GREATEREQI;
  case LESSERI:
    return GREATERI;
  case GREATEREQI:
    return LESSEREQI;
  case GREATERI:
    return LESSERI;
  default:
    return -1;
  }
}
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
  case EQUALS:
    return EQUALSI;
  case NOTEQUALS:
    return NOTEQUALSI;
  case GREATER:
    return GREATERI;
  case GREATEREQ:
    return GREATEREQI;
  case LESSER:
    return LESSERI;
  case LESSEREQ:
    return LESSEREQI;
  default:
    return -1;
  }
}
int optimizeLoadAdressRecur(instrList *ir, char *id, char *str, int *changed) {

  if (!ir->next)
    return 1;

  switch (ir->next->instr.opcode) {
  case LABEL:
  case COND:
  case SAVEREGISTERS:
  case LOADREGISTERS:
  case EQUALSI:
  case NOTEQUALSI:
  case GREATEREQI:
  case GREATERI:
  case LESSERI:
  case LESSEREQI:
  case EQUALS:
  case NOTEQUALS:
  case GREATEREQ:
  case GREATER:
  case LESSER:
  case LESSEREQ:
  case BNEZ:
    return 1;
  case LOADADRESS: {
    if (!strcmp(id, ir->next->instr.arg1)) {
      *changed = 1;
      ir->instr.opcode = NOP;
      return optimizeLoadAdressRecur(ir->next, id, ir->next->instr.arg2,
                                     changed);
    }
  } break;
  case MOVE: {
    char *id1 = ir->next->instr.arg1;
    char *id2 = ir->next->instr.arg2;
    if (!strcmp(id, id1))
      return 1;
    if (!strcmp(id, id2)) {
      ir->next->instr.arg2 = str;
      ir->next->instr.opcode = LOADADRESS;
      *changed = 1;
    }
  } break;
  default:
    break;
  }
  return optimizeLoadAdressRecur(ir->next, id, str, changed);
}
int optimizeLoadAdress(instrList *ir, int *changed) {
  if (!ir->next || !ir->next->next)
    return 1;
  int ret =
      optimizeLoadAdressRecur(ir, ir->instr.arg1, ir->instr.arg2, changed);
  if (!ret) {
    fprintf(stderr, "Unable to apply MoveI optimizatiion\n");
    return 0;
  }
  return ret;
}

int optimizeCond(instrList *ir, char *label) {
  if (!ir->next)
    return 1;

  switch (ir->instr.opcode) {
  case LABEL:
    if (!strcmp(ir->instr.arg1, label))
      return 1;
    break;
  default:
    ir->instr.opcode = NOP;
    break;
  }
  return optimizeCond(ir->next, label);
}
int optimizeMoveIRecur(instrList *ir, stringLiterals **strs,
                       floatLiterals **floats, char *id, int val,
                       int *changed) {

  switch (ir->instr.opcode) {
  case SAVEREGISTERS:
  case LOADREGISTERS:
  case LABEL:
  case LOADADRESS:
  case COND:
    return 0;
    break;
  // case BNEZ:
  case EQUALSI:
  case NOTEQUALSI:
  case GREATEREQI:
  case GREATERI:
  case LESSERI:
  case LESSEREQI: {
    char *t0 = ir->instr.arg1;
    if (!strcmp(t0, id)) {
      switch (ir->instr.opcode) {
      case EQUALSI:
        ir->instr.num = ir->instr.num == val;
        break;
      case NOTEQUALSI:
        ir->instr.num = ir->instr.num != val;
        break;
      case GREATEREQI:
        ir->instr.num = val >= ir->instr.num;
        break;
      case GREATERI:
        ir->instr.num = val > ir->instr.num;
        break;
      case LESSERI:
        ir->instr.num = val < ir->instr.num;
        break;
      case LESSEREQI:
        ir->instr.num = val <= ir->instr.num;
        break;
      }
      ir->instr.opcode = NOP;
      *changed = 1;
      char *labelEffective = ir->instr.arg3;
      if (!ir->instr.num)
        labelEffective = ir->instr.arg4;
      printf("%d %s %s %s\n", ir->instr.num, labelEffective, ir->instr.arg3,
             ir->instr.arg4);
      if (!optimizeCond(ir, labelEffective)) {
        fprintf(stderr, "Error while optimizing condition\n");
        return -1;
      }
    }
  } break;
  case GREATEREQ:
  case GREATER:
  case LESSER:
  case LESSEREQ: {

    char *t0 = ir->instr.arg1;
    char *t1 = ir->instr.arg2;
    if (!strcmp(t0, id)) {
      ir->instr.opcode = reverseCondition(convertToImmediate(ir->instr.opcode));
      ir->instr.arg1 = t1;
      ir->instr.num = val;
      *changed = 1;
    } else if (!strcmp(t1, id)) {
      ir->instr.opcode = convertToImmediate(ir->instr.opcode);
      ir->instr.num = val;
      *changed = 1;
    }
  } break;

  case EQUALS:
  case NOTEQUALS: {
    char *t0 = ir->instr.arg1;
    char *t1 = ir->instr.arg2;
    if (!strcmp(t0, id)) {
      ir->instr.opcode = convertToImmediate(ir->instr.opcode);
      ir->instr.arg1 = t1;
      ir->instr.num = val;
      *changed = 1;
    } else if (!strcmp(t1, id)) {
      ir->instr.opcode = convertToImmediate(ir->instr.opcode);
      ir->instr.num = val;
      *changed = 1;
    }
  } break;
  case MOVE:
  case ADDI:
  case SUBI:
  case MULTI:
  case DIVIDEI:
  case POWERI: {
    char *t0 = ir->instr.arg1;
    char *t1 = ir->instr.arg2;
    if (!strcmp(t0, id))
      return 1;
    else if (!strcmp(t1, id)) {
      switch (ir->instr.opcode) {
      case MOVE:
        ir->instr.num = val;
        break;
      case ADDI:
        ir->instr.num = val + ir->instr.num;
        break;
      case DIVIDEI:
        ir->instr.num = val / ir->instr.num;
        break;
      case SUBI:
        ir->instr.num = val - ir->instr.num;
        break;
      case MULTI:
        ir->instr.num = val * ir->instr.num;
        break;
      case POWERI:
        ir->instr.num = (int)floor(pow(val, ir->instr.num));
        break;
      default:
        fprintf(stderr, "Not supposed to happend\n");
        return -1;
      }
      *changed = 1;
      ir->instr.opcode = MOVEI;
    }
  } break;
  case SUB:
  case DIVIDE:
  case POWER: {
    char *t0 = ir->instr.arg1;
    char *t1 = ir->instr.arg2;
    char *t2 = ir->instr.arg3;
    if (!strcmp(t0, id))
      return 0;
    else if (!strcmp(t1, id))
      return 1;
    else if (!strcmp(t2, id)) {
      ir->instr.opcode = convertToImmediate(ir->instr.opcode);
      ir->instr.num = val;
      *changed = 1;
    }
  } break;
  case ADD:
  case MULT: {
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
    } else if (!strcmp(t2, id)) {
      ir->instr.opcode = convertToImmediate(ir->instr.opcode);
      ir->instr.num = val;
      *changed = 1;
    }
  } break;
  case MOVEI: {
    char *t0 = ir->instr.arg1;
    if (!strcmp(t0, id))
      return 1;
  } break;
  default:
    break;
  }
  if (!ir->next)
    return 0;
  return optimizeMoveIRecur(ir->next, strs, floats, id, val, changed);
}
int optimizeMoveI(instrList *ir, stringLiterals **strs, floatLiterals **floats,
                  int *changed) {
  if (!ir->next || !ir->next->next)
    return 0;
  instrList *next = ir->next;
  int ret = optimizeMoveIRecur(next, strs, floats, ir->instr.arg1,
                               ir->instr.num, changed);
  if (ret == -1) {
    fprintf(stderr, "Unable to apply MoveI optimizatiion\n");
    return -1;
  }
  return ret;
}

int optimizeArith(instrList *ir, stringLiterals **strs, floatLiterals **floats,
                  int *changed) {
  if (!ir->next)
    return 1;

  switch (ir->next->instr.opcode) {
  case LABEL:
  case COND:
  case LOADADRESS:
  case SAVEREGISTERS:
  case LOADREGISTERS:
  case EQUALSI:
  case NOTEQUALSI:
  case GREATEREQI:
  case GREATERI:
  case LESSERI:
  case LESSEREQI:
  case EQUALS:
  case NOTEQUALS:
  case GREATEREQ:
  case GREATER:
  case LESSER:
  case LESSEREQ:
  case BNEZ:
    return 1;
    break;
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

  instrList *head = ir;
  // printInstructions(ir);
  while (head) {
    switch (head->instr.opcode) {
    case LOADADRESS: {
      int tChanged = 0;
      if (!optimizeLoadAdress(head, &tChanged)) {
        fprintf(stderr, "Optimizer error while optimizing LoadAdress\n");
        return NULL;
      }
      if (tChanged) {
        *changed = 1;
        head->instr.opcode = NOP;
      }
    } break;
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
    case MOVEI: {
      int moveApplied = 0;
      int ret = optimizeMoveI(head, strs, floats, &moveApplied);
      if (ret == -1) {
        fprintf(stderr, "Optimizer error while optimizing MOVEI\n");
        return NULL;
      } else if (moveApplied)
        *changed = 1;

      // if (!ret)
      //   head->instr.opcode = NOP;

    } break;
    case EQUALS:
    case NOTEQUALS:
    case GREATEREQ:
    case GREATER:
    case LESSER:
    case LESSEREQ:
    case BNEZ:
      break;

    default:
      break;
    }
    head = head->next;
  }
  // printInstructions(ir);

  return ir;
}
