#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ir.h"

static int temp_count = 0;
static int label_count = 0;
static InstrList *code_list = NULL;
static InstrList *last_instr = NULL;
static char *condLeft = NULL;
static char *condRight = NULL;

static char *temps[18] = {"t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7", "t8",
                          "t9", "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7"};
static int used[18];
char *getVarTemp(char *id, struct vars *vars) {
  if (!vars || id == NULL)
    return NULL;
  while (vars != NULL) {
    if (strcmp(id, vars->id) == 0)
      return vars->temp;

    vars = vars->next;
  }
  return NULL;
}
void removeTemp(char *id) {
  if (!id)
    return;
  for (int i = 0; i < 18; i++) {
    if (strcmp(id, temps[i]) == 0) {
      used[i] = 0;
      temp_count--;
      return;
    }
  }
}

void emitFunction(char *id, char *temp, char *temp2) {

  Instruction instr = {CALL, id, temp, temp2, NULL, 0};
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
void emit2(Opcode opc, char *arg1, char *arg2) {
  Instruction instr = {opc, arg1, arg2, NULL, NULL, 0};
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
  Instruction instr = {opc, arg1, arg2, arg3, NULL, 0};
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
  Instruction instr = {MOVEI, dest, NULL, NULL, NULL, num};
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

void emitOp(op ope, char *dest, char *src1, char *src2) {
  Instruction instr = {OP, dest, src1, src2, NULL, 0, ope};
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
  Instruction instr = {LABEL, label, NULL, NULL, NULL, 0};
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
  Instruction instr = {JUMP, label, NULL, NULL, NULL, 0};
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
  Instruction instr = {COND, src1, src2, label1, label2, 0, op};
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
  char *temp = NULL;
  for (int i = 0; i < 18; i++) {
    if (used[i] == 0) {
      printf("%s %i\n", temps[i], used[i]);
      used[i] = 1;
      temp = strdup(temps[i]);
      temp_count++;
      return temp;
    }
  }

  return temp;
}

char *newLabel() {
  char *label = malloc(10);
  sprintf(label, "L%d", label_count++);
  return label;
}

struct vars *addNode(char *id, char *temp, struct vars *vars) {
  if (!vars) {
    vars = (struct vars *)malloc(sizeof(struct vars));
    vars->next = NULL;
    vars->id = strdup(id);
    if (!temp)
      vars->temp = newTemp();
    else
      vars->temp = strdup(temp);
    return vars;
  }
  struct vars *head = vars;
  while (vars->next)
    vars = vars->next;

  vars->next = (struct vars *)malloc(sizeof(struct vars));
  vars->next->next = NULL;

  vars->next->id = strdup(id);
  if (!temp)
    vars->next->temp = newTemp();
  else
    vars->next->temp = strdup(temp);
  return head;
}
struct vars *transVarDecl(Stm varDecl, struct vars *vars) {
  if (!varDecl)
    return vars;
  if (varDecl->compound.fst->tag == NUM) {

    char *id = strdup(varDecl->compound.fst->assign.ident);
    char *temp = newTemp();
    vars = addNode(id, temp, vars);
    transExp(varDecl->compound.fst->assign.expr, vars->temp, vars);
  } else if (varDecl->compound.fst->tag == STRLITERAL) {

    char *id = strdup(varDecl->compound.fst->assign.ident);
    char *temp = id;
    vars = addNode(id, temp, vars);
    transExp(varDecl->compound.fst->assign.expr, vars->temp, vars);
  }
  return transVarDecl(varDecl->compound.snd, vars);
}
void printVars(struct vars *vars) {
  struct vars *head = vars;
  while (head) {

    printf("%s %s\n", head->id, head->temp);
    head = head->next;
  }
}
InstrList *genCode(Prog program) {

  temp_count = 0;
  label_count = 0;
  code_list = NULL;
  last_instr = NULL;
  for (int i = 0; i < 18; i++)
    used[i] = 0;
  struct vars *vars = transVarDecl(program->varDec, NULL);
  printVars(vars);

  transStm(program->statements, vars);

  return code_list;
}

void transStm(Stm stm, struct vars *vars) {
  if (stm == NULL)
    return;

  switch (stm->tag) {
  case ASSIGN: {
    char *id = getVarTemp(stm->assign.ident, vars);
    if (!id) {
      fprintf(stderr, "Unable to find variable to bind the value\n");
      return;
    }
    char *temp = NULL;
    int remove = 0;
    if (stm->assign.expr->tag == ID) {

      temp = getVarTemp(stm->assign.expr->id, vars);
      if (!temp) {
        fprintf(stderr, "Unable to find variable to bind the value\n");
        return;
      }
    } else {

      temp = newTemp();
      remove = 1;
    }
    transExp(stm->assign.expr, temp, vars);
    emit2(MOVE, id, temp);
    if (remove)
      removeTemp(temp);

    break;
  }
  case COMPOUND:
    transStm(stm->compound.fst, vars);
    transStm(stm->compound.snd, vars);
    break;
  case IF: {
    char *temp = NULL;
    char *label_true = newLabel();
    char *label_false = newLabel();
    char *label_end = newLabel();

    transExp(stm->ifStmt.cond, temp, vars);
    emitCond(stm->ifStmt.cond->binop.op, condLeft, condRight, label_true,
             label_false);

    emitLabel(label_true);
    transStm(stm->ifStmt.thenBranch, vars);
    emitJump(label_end);

    emitLabel(label_false);
    if (stm->ifStmt.elseBranch != NULL) {
      transStm(stm->ifStmt.elseBranch, vars);
    }
    emitJump(label_end);

    emitLabel(label_end);
    break;
  }
  case WHILE: {
    char *temp = NULL;
    char *label_start = newLabel();
    char *label_body = newLabel();
    char *label_end = newLabel();

    emitLabel(label_start);
    transExp(stm->whileStmt.cond, temp, vars);
    emitCond(NEQ, condLeft, condRight, label_body, label_end);

    emitLabel(label_body);
    transStm(stm->whileStmt.body, vars);
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

  case FUNCTION: {

    char *temp = "a0";
    char *temp2 = NULL;
    transExp(stm->function.args->arg, temp, vars);
    if (stm->function.args->nextArg) {
      temp2 = "a1";

      transExp(stm->function.args->nextArg->arg, temp2, vars);
    }
    emitFunction(stm->function.ident, temp, temp2);
  } break;
  }
}

void transExp(Exp exp, char *dest, struct vars *vars) {
  if (!exp)
    return;

  switch (exp->tag) {

  case NUM:
    emitMoveI(dest, (int)exp->val);
    break;
  case ID: {
    char *id = getVarTemp(exp->id, vars);
    if (id == NULL)
      id = exp->id;
    if (strcmp(dest, id) == 0)
      break;
    emit2(MOVE, dest, id);
    break;
  }
  case BOOL:
    emitMoveI(dest, exp->bool_val);
    break;
  case OP:
    transBinOp(exp, dest, vars);
    break;
  case STRLITERAL:
    emit2(MOVEI, dest, exp->str);
    break;
  }
}

void transBinOp(Exp exp, char *dest, struct vars *vars) {
  if (dest == NULL) {

    condLeft = newTemp();
    condRight = newTemp();

    transExp(exp->binop.left, condLeft, vars);
    transExp(exp->binop.right, condRight, vars);
    removeTemp(condLeft);
    removeTemp(condRight);
    return;
  }
  char *t1 = newTemp();
  char *t2 = newTemp();

  transExp(exp->binop.left, t1, vars);
  transExp(exp->binop.right, t2, vars);
  removeTemp(t1);
  removeTemp(t2);

  emitOp(exp->binop.op, dest, t1, t2);
}

void printInstructions(InstrList *list) {
  printf("\nIR:\n");
  InstrList *current = list;
  while (current != NULL) {
    switch (current->instr.opcode) {
    case MOVE:
      printf("MOVE %s %s\n", current->instr.arg1, current->instr.arg2);
      break;
    case CALL:
      if (current->instr.arg3 != NULL) {

        printf("CALL %s %s %s\n", current->instr.arg1, current->instr.arg2,
               current->instr.arg3);
      } else
        printf("CALL %s %s\n", current->instr.arg1, current->instr.arg2);
      break;
    case MOVEI:
      printf("MOVEI %s %d\n", current->instr.arg1, current->instr.num);
      break;
    case OP: {
      char *op_str;
      switch (current->instr.binop) {
      case XOR:
        op_str = "^";
        break;
      case NOT:
        op_str = "!";
        break;
      case POW:
        op_str = "POW";
        break;
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
      }
      printf("%s := %s %s %s\n", current->instr.arg1, current->instr.arg2,
             op_str, current->instr.arg3);
      break;
    }
    case LABEL:
      printf("%s:\n", current->instr.arg1);
      break;
    case JUMP:
      printf("JUMP %s\n", current->instr.arg1);
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
      printf("COND %s %s %s %s %s\n", current->instr.arg1, op_str,
             current->instr.arg2, current->instr.arg3, current->instr.arg4);
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
