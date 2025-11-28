#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ir.h"

static int temp_count = 0;
static int label_count = 0;
static InstrList *code_list = NULL;
static InstrList *last_instr = NULL;

void emit2(Opcode opc, char *arg1, char *arg2) {
  Instruction instr = {opc, arg1, arg2, NULL, 0};
  InstrList *new_node = malloc(sizeof(InstrList));
  new_node->instr = instr;
  new_node->next = NULL;

  if (code_list == NULL) {
    code_list = new_node;
    last_instr = new_node;
  } else {
    last_instr->next = new_node;
    last_instr = new_node;
  }
}

void emit3(Opcode opc, char *arg1, char *arg2, char *arg3) {
  Instruction instr = {opc, arg1, arg2, arg3, 0};
  InstrList *new_node = malloc(sizeof(InstrList));
  new_node->instr = instr;
  new_node->next = NULL;

  if (code_list == NULL) {
    code_list = new_node;
    last_instr = new_node;
  } else {
    last_instr->next = new_node;
    last_instr = new_node;
  }
}

void emitMoveI(char *dest, int num) {
  Instruction instr = {MOVEI, dest, NULL, NULL, num};
  InstrList *new_node = malloc(sizeof(InstrList));
  new_node->instr = instr;
  new_node->next = NULL;

  if (code_list == NULL) {
    code_list = new_node;
    last_instr = new_node;
  } else {
    last_instr->next = new_node;
    last_instr = new_node;
  }
}

void emitOp(op op, char *dest, char *src1, char *src2) {
  Instruction instr = {OP, dest, src1, src2, 0, op};
  InstrList *new_node = malloc(sizeof(InstrList));
  new_node->instr = instr;
  new_node->next = NULL;

  if (code_list == NULL) {
    code_list = new_node;
    last_instr = new_node;
  } else {
    last_instr->next = new_node;
    last_instr = new_node;
  }
}

void emitLabel(char *label) {
  Instruction instr = {LABEL, label, NULL, NULL, 0};
  InstrList *new_node = malloc(sizeof(InstrList));
  new_node->instr = instr;
  new_node->next = NULL;

  if (code_list == NULL) {
    code_list = new_node;
    last_instr = new_node;
  } else {
    last_instr->next = new_node;
    last_instr = new_node;
  }
}

void emitJump(char *label) {
  Instruction instr = {JUMP, label, NULL, NULL, 0};
  InstrList *new_node = malloc(sizeof(InstrList));
  new_node->instr = instr;
  new_node->next = NULL;

  if (code_list == NULL) {
    code_list = new_node;
    last_instr = new_node;
  } else {
    last_instr->next = new_node;
    last_instr = new_node;
  }
}

void emitCond(op op, char *src1, char *src2, char *label1, char *label2) {
  Instruction instr = {COND, src1, src2, label1, 0, op};
  InstrList *new_node = malloc(sizeof(InstrList));
  new_node->instr = instr;
  new_node->next = NULL;

  if (code_list == NULL) {
    code_list = new_node;
    last_instr = new_node;
  } else {
    last_instr->next = new_node;
    last_instr = new_node;
  }
}

char *newTemp() {
  char *temp = malloc(10);
  sprintf(temp, "t%d", temp_count++);
  return temp;
}

char *newLabel() {
  char *label = malloc(10);
  sprintf(label, "L%d", label_count++);
  return label;
}

InstrList *genCode(Stm program) {

  temp_count = 0;
  label_count = 0;
  code_list = NULL;
  last_instr = NULL;

  transStm(program);

  return code_list;
}

void transStm(Stm stm) {
  if (stm == NULL)
    return;

  switch (stm->tag) {
  case ASSIGN: {
    char *temp = newTemp();
    transExp(stm->assign.expr, temp);
    emit2(MOVE, stm->assign.ident, temp);
    break;
  }
  case COMPOUND:
    transStm(stm->compound.fst);
    transStm(stm->compound.snd);
    break;
  case IF: {
    char *temp = newTemp();
    char *label_true = newLabel();
    char *label_false = newLabel();
    char *label_end = newLabel();

    transExp(stm->ifStmt.cond, temp);
    emitCond(NEQ, temp, "0", label_true, label_false);

    emitLabel(label_true);
    transStm(stm->ifStmt.thenBranch);
    emitJump(label_end);

    emitLabel(label_false);
    if (stm->ifStmt.elseBranch != NULL) {
      transStm(stm->ifStmt.elseBranch);
    }
    emitJump(label_end);

    emitLabel(label_end);
    break;
  }
  case WHILE: {
    char *temp = newTemp();
    char *label_start = newLabel();
    char *label_body = newLabel();
    char *label_end = newLabel();

    emitLabel(label_start);
    transExp(stm->whileStmt.cond, temp);
    emitCond(NEQ, temp, "0", label_body, label_end);

    emitLabel(label_body);
    transStm(stm->whileStmt.body);
    emitJump(label_start);

    emitLabel(label_end);
    break;
  }
  case INCR: {

    char *temp1 = newTemp();
    char *temp2 = newTemp();
    emit2(MOVE, temp1, stm->ident);
    emitMoveI(temp2, 1);
    emitOp(PLUS, stm->ident, temp1, temp2);
    break;
  }

  default:
    break;
  }
}

void transExp(Exp exp, char *dest) {
  if (exp == NULL)
    return;

  switch (exp->tag) {
  case NUM:
    emitMoveI(dest, (int)exp->val);
    break;
  case ID:
    emit2(MOVE, dest, exp->id);
    break;
  case BOOL:
    emitMoveI(dest, exp->bool_val);
    break;
  case OP:
    transBinOp(exp, dest);
    break;
  case STRLITERAL:

    emit2(MOVE, dest, exp->str);
    break;
  }
}

void transBinOp(Exp exp, char *dest) {
  char *t1 = newTemp();
  char *t2 = newTemp();

  transExp(exp->binop.left, t1);
  transExp(exp->binop.right, t2);

  emitOp(exp->binop.op, dest, t1, t2);
}

void printInstructions(InstrList *list) {
  printf("\nIR:\n");
  InstrList *current = list;
  while (current != NULL) {
    switch (current->instr.opcode) {
    case MOVE:
      printf("%s := %s\n", current->instr.arg1, current->instr.arg2);
      break;
    case MOVEI:
      printf("%s := %d\n", current->instr.arg1, current->instr.num);
      break;
    case OP: {
      char *op_str;
      switch (current->instr.binop) {
      case PLUS:
        op_str = "+";
        break;
      case MINUS:
        op_str = "-";
        break;
      case TIMES:
        op_str = "*";
        break;
      case DIV:
        op_str = "/";
        break;
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
      printf("%s := %s %s %s\n", current->instr.arg1, current->instr.arg2,
             op_str, current->instr.arg3);
      break;
    }
    case LABEL:
      printf("%s:\n", current->instr.arg1);
      break;
    case JUMP:
      printf("goto %s\n", current->instr.arg1);
      break;
    case COND: {
      char *op_str;
      switch (current->instr.binop) {
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
      printf("if %s %s %s goto %s\n", current->instr.arg1, op_str,
             current->instr.arg2, current->instr.arg3);
      break;
    }
    }
    current = current->next;
  }
}

void freeInstructions(InstrList *list) {
  InstrList *current = list;
  while (current != NULL) {
    InstrList *next = current->next;
    free(current->instr.arg1);
    free(current->instr.arg2);
    free(current->instr.arg3);
    free(current);
    current = next;
  }
}
