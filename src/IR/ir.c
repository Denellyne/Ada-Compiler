#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ir.h"

static int tempCount = 0;
static int labelCount = 0;
static int staticStrCount = 0;
static InstrList *codeList = NULL;
static InstrList *lastInstr = NULL;
static char *condLeft = NULL;
static char *condRight = NULL;

static char *temps[18] = {"t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7", "t8",
                          "t9", "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7"};
static int used[18];

char *getVarTemp(char *id, struct vars *vars) {
  if (!vars || !id)
    return NULL;
  while (vars) {
    if (strcmp(id, vars->id) == 0)
      return vars->temp;

    vars = vars->next;
  }
  return NULL;
}

char *newTemp() {
  for (int i = 0; i < 18; i++) {
    if (used[i] == 0) {
      used[i] = 1;
      tempCount++;
      assert(tempCount < 18);
      return strdup(temps[i]);
    }
  }

  return NULL;
}
int searchStrLiteralById(char *id, stringLiterals *strs) {
  stringLiterals *head = strs;
  while (head) {
    if (!strcmp(id, head->id))
      return 1;
    head = head->next;
  }
  return 0;
}

stringLiterals *addString(char *id, char *str, stringLiterals *strs) {
  if (!strs) {
    strs = (stringLiterals *)malloc(sizeof(stringLiterals));
    strs->next = NULL;
    strs->id = strdup(id);
    if (!str)
      return NULL;
    else
      strs->str = strdup(str);
    return strs;
  }
  stringLiterals *head = strs;
  while (strs->next)
    strs = strs->next;

  strs->next = (stringLiterals *)malloc(sizeof(stringLiterals));
  strs->next->next = NULL;

  strs->next->id = strdup(id);
  if (!str)
    return NULL;
  else
    strs->next->str = strdup(str);
  return head;
}
char *newStaticString() {
  char *id = malloc(10);
  sprintf(id, "str%d", staticStrCount++);

  return id;
}

void removeTemp(char *temp) {
  if (!temp)
    return;
  for (int i = 0; i < 18; i++) {
    if (strcmp(temp, temps[i]) == 0) {
      used[i] = 0;
      tempCount--;
      assert(tempCount >= 0);
      return;
    }
  }
}

int emitFunction(char *id, char *temp, char *temp2) {

  Instruction instr = {CALL, id, temp, temp2, NULL, 0};
  InstrList *newNode = malloc(sizeof(InstrList));
  if (!newNode)
    return 0;
  newNode->instr = instr;
  newNode->next = NULL;

  if (codeList == NULL) {
    codeList = newNode;
    lastInstr = newNode;
  } else {
    lastInstr->next = newNode;
    lastInstr = newNode;
  }
  return 1;
}
int emit2(Opcode opc, char *arg1, char *arg2) {
  Instruction instr = {opc, arg1, arg2, NULL, NULL, 0};
  InstrList *newNode = malloc(sizeof(InstrList));
  if (!newNode)
    return 0;
  newNode->instr = instr;
  newNode->next = NULL;

  if (codeList == NULL) {
    codeList = newNode;
    lastInstr = newNode;
  } else {
    lastInstr->next = newNode;
    lastInstr = newNode;
  }
  return 1;
}

int emit3(Opcode opc, char *arg1, char *arg2, char *arg3) {
  Instruction instr = {opc, arg1, arg2, arg3, NULL, 0};
  InstrList *newNode = malloc(sizeof(InstrList));
  if (!newNode)
    return 0;
  newNode->instr = instr;
  newNode->next = NULL;

  if (codeList == NULL) {
    codeList = newNode;
    lastInstr = newNode;
  } else {
    lastInstr->next = newNode;
    lastInstr = newNode;
  }
  return 1;
}

int emitMoveI(char *dest, int num) {
  Instruction instr = {MOVEI, dest, NULL, NULL, NULL, num};
  InstrList *newNode = malloc(sizeof(InstrList));
  if (!newNode)
    return 0;
  newNode->instr = instr;
  newNode->next = NULL;

  if (codeList == NULL) {
    codeList = newNode;
    lastInstr = newNode;
  } else {
    lastInstr->next = newNode;
    lastInstr = newNode;
  }
  return 1;
}

int convertOp(const op ope, const int immediate) {
  switch (ope) {
  case POW:
    if (immediate)
      return POWERI;
    return POWER;

  case PLUS:
    if (immediate)
      return ADDI;
    return ADD;
  case MINUS:
    if (immediate)
      return SUBI;
    return SUB;
  case TIMES:
    if (immediate)
      return MULTI;
    return MULT;
  case DIV:
    if (immediate)
      return DIVIDEI;
    return DIVIDE;
  case AND:
  case OR:
  case NOT:
  case XOR:
  case EQ:
  case NEQ:
  case LT:
  case GT:
  case LE:
  case GE:
    return ope;
    break;
  }

  return -1;
}
int emitOp(op ope, char *dest, char *src1, char *src2, int val) {
  int opConverted = convertOp(ope, src2 == NULL);
  if (opConverted == -1) {
    fprintf(stderr, "Unable to convert operand\n");
    return 0;
  }
  Instruction instr = {opConverted, dest, src1, src2, NULL, val, 0};
  InstrList *newNode = malloc(sizeof(InstrList));
  if (!newNode)
    return 0;
  newNode->instr = instr;
  newNode->next = NULL;

  if (codeList == NULL) {
    codeList = newNode;
    lastInstr = newNode;
  } else {
    lastInstr->next = newNode;
    lastInstr = newNode;
  }
  return 1;
}

int emitLabel(char *label) {
  Instruction instr = {LABEL, label, NULL, NULL, NULL, 0};
  InstrList *newNode = malloc(sizeof(InstrList));
  if (!newNode)
    return 0;
  newNode->instr = instr;
  newNode->next = NULL;

  if (codeList == NULL) {
    codeList = newNode;
    lastInstr = newNode;
  } else {
    lastInstr->next = newNode;
    lastInstr = newNode;
  }
  return 1;
}

int emitJump(char *label) {
  Instruction instr = {JUMP, label, NULL, NULL, NULL, 0};
  InstrList *newNode = malloc(sizeof(InstrList));
  if (!newNode)
    return 0;
  newNode->instr = instr;
  newNode->next = NULL;

  if (codeList == NULL) {
    codeList = newNode;
    lastInstr = newNode;
  } else {
    lastInstr->next = newNode;
    lastInstr = newNode;
  }
  return 1;
}

int emitCond(op op, char *src1, char *src2, char *label1, char *label2) {
  Instruction instr = {COND, src1, src2, label1, label2, 0, op};
  InstrList *newNode = malloc(sizeof(InstrList));
  if (!newNode)
    return 0;
  newNode->instr = instr;
  newNode->next = NULL;

  if (codeList == NULL) {
    codeList = newNode;
    lastInstr = newNode;
  } else {
    lastInstr->next = newNode;
    lastInstr = newNode;
  }
  return 1;
}

char *newLabel() {
  char *label = malloc(10);
  sprintf(label, "L%d", labelCount++);
  return label;
}

struct vars *addNode(char *id, char *temp, struct vars *vars) {
  if (!vars) {
    vars = (struct vars *)malloc(sizeof(struct vars));
    vars->next = NULL;
    vars->id = strdup(id);
    if (!temp)
      return NULL;

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
    return NULL;
  else
    vars->next->temp = strdup(temp);
  return head;
}
struct vars *transVarDecl(Stm varDecl, struct vars *vars, stringLiterals **strs,
                          int *error) {
  if (*error)
    return NULL;
  if (!varDecl)
    return vars;
  if (varDecl->compound.fst->assign.type == NUM) {

    char *id = strdup(varDecl->compound.fst->assign.ident);
    char *temp = newTemp();
    vars = addNode(id, temp, vars);
    if (!vars) {
      fprintf(stderr, "Error while addind node to vars struct\n");
      *error = 1;
      return NULL;
    }
    transExp(varDecl->compound.fst->assign.expr, temp, vars, strs);
  }
  if (varDecl->compound.fst->assign.type == BOOL) {

    char *id = strdup(varDecl->compound.fst->assign.ident);
    char *temp = newTemp();
    vars = addNode(id, temp, vars);
    if (!vars) {
      fprintf(stderr, "Error while addind node to vars struct\n");
      *error = 1;
      return NULL;
    }
    transExp(varDecl->compound.fst->assign.expr, temp, vars, strs);
  }
  if (varDecl->compound.fst->assign.type == STRLITERAL) {

    char *id = strdup(varDecl->compound.fst->assign.ident);
    char *temp = id;
    vars = addNode(id, temp, vars);
    if (!vars) {
      fprintf(stderr, "Error while addind node to vars struct\n");
      *error = 1;
      return NULL;
    }
    *strs = addString(id, varDecl->compound.fst->assign.expr->str, *strs);
    // transExp(varDecl->compound.fst->assign.expr, vars->temp, vars, strs);
  }
  return transVarDecl(varDecl->compound.snd, vars, strs, error);
}
void printVars(struct vars *vars) {
  struct vars *head = vars;
  while (head) {

    printf("%s %s\n", head->id, head->temp);
    head = head->next;
  }
}
InstrList *genCode(Prog program, stringLiterals **strs) {

  staticStrCount = 0;
  tempCount = 0;
  labelCount = 0;
  codeList = NULL;
  lastInstr = NULL;
  int error = 0;
  stringLiterals *strsLocal = NULL;

  for (int i = 0; i < 18; i++)
    used[i] = 0;

  struct vars *vars = transVarDecl(program->varDec, NULL, &strsLocal, &error);
  if (error)
    return NULL;
  // printVars(vars);

  transStm(program->statements, vars, &strsLocal);
  *strs = strsLocal;

  return codeList;
}

int transStm(Stm stm, struct vars *vars, stringLiterals **strs) {
  if (!stm)
    return 0;

  switch (stm->tag) {
  case ASSIGN: {
    char *id = getVarTemp(stm->assign.ident, vars);
    if (!id) {
      fprintf(stderr, "Unable to find variable to bind the value\n");
      return 0;
    }
    char *temp = NULL;
    int remove = 0;
    if (stm->assign.expr->tag == ID) {

      temp = getVarTemp(stm->assign.expr->id, vars);
      if (!temp) {
        fprintf(stderr, "Unable to find variable to bind the value\n");
        return 0;
      }
    } else {

      temp = newTemp();
      remove = 1;
    }
    int ret =
        transExp(stm->assign.expr, temp, vars, strs) && emit2(MOVE, id, temp);
    if (remove)
      removeTemp(temp);
    return ret;

    break;
  }
  case COMPOUND:
    return transStm(stm->compound.fst, vars, strs) &&
           transStm(stm->compound.snd, vars, strs);
    break;
  case IF: {
    char *temp = NULL;
    char *label_true = newLabel();
    char *label_false = newLabel();
    char *label_end = newLabel();
    printf("a\n");

    int ret = transExp(stm->ifStmt.cond, temp, vars, strs) &&
              emitCond(stm->ifStmt.cond->binop.op, condLeft, condRight,
                       label_true, label_false) &&

              emitLabel(label_true) &&
              transStm(stm->ifStmt.thenBranch, vars, strs) &&
              emitJump(label_end) &&

              emitLabel(label_false);
    if (stm->ifStmt.elseBranch != NULL) {
      ret = ret && transStm(stm->ifStmt.elseBranch, vars, strs);
    }
    return ret && emitJump(label_end) &&

           emitLabel(label_end);

    break;
  }
  case WHILE: {
    char *temp = NULL;
    char *label_start = newLabel();
    char *label_body = newLabel();
    char *label_end = newLabel();

    return emitLabel(label_start) &&
           transExp(stm->whileStmt.cond, temp, vars, strs) &&
           emitCond(stm->ifStmt.cond->binop.op, condLeft, condRight, label_body,
                    label_end) &&

           emitLabel(label_body) && transStm(stm->whileStmt.body, vars, strs) &&
           emitJump(label_start) &&

           emitLabel(label_end);

    break;
  }
  case INCR: {

    char *temp1 = newTemp();
    char *temp2 = newTemp();
    return emit2(MOVE, temp1, stm->ident) && emitMoveI(temp2, 1) &&
           emitOp(PLUS, stm->ident, temp1, temp2, 0);
    break;
  }

  case FUNCTION: {

    char *temp = "a0";
    char *temp2 = NULL;
    int ret = transExp(stm->function.args->arg, temp, vars, strs);
    if (stm->function.args->nextArg) {
      temp2 = "a1";

      ret =
          ret && transExp(stm->function.args->nextArg->arg, temp2, vars, strs);
    }
    return emitFunction(stm->function.ident, temp, temp2);
  } break;
  }
}

int transExp(Exp exp, char *dest, struct vars *vars, stringLiterals **strs) {
  if (!exp)
    return 0;
  else if (!dest && exp->tag == NUM)
    return -1;

  switch (exp->tag) {

  case NUM:
    return emitMoveI(dest, (int)exp->val);
    break;
  case ID: {
    if (searchStrLiteralById(exp->id, *strs))
      return emit2(LOADADRESS, dest, exp->id);
    char *id = getVarTemp(exp->id, vars);
    if (id == NULL) {

      id = exp->id;
    }
    if (strcmp(dest, id) == 0)
      break;
    return emit2(MOVE, dest, id);
    break;
  }
  case BOOL:
    return emitMoveI(dest, exp->bool_val);
    break;
  case OP:
    return transBinOp(exp, dest, vars, strs);
    break;
  case STRLITERAL: {
    char *id = getVarTemp(exp->id, vars);
    if (!id) {
      id = newStaticString();
      *strs = addString(id, exp->str, *strs);
      if (!*strs)
        return 0;
    }
    if (!id) {
      fprintf(stderr, "Unable to create static string for string literal\n");
      return 0;
    }
    return emit2(LOADADRESS, dest, id);
  } break;
  }
  return 1;
}

int transBinOp(Exp exp, char *dest, struct vars *vars, stringLiterals **strs) {
  if (dest == NULL) {

    condLeft = newTemp();
    condRight = newTemp();

    transExp(exp->binop.left, condLeft, vars, strs);
    transExp(exp->binop.right, condRight, vars, strs);
    removeTemp(condLeft);
    removeTemp(condRight);
    return 1;
  }
  char *t1 = newTemp();

  transExp(exp->binop.left, t1, vars, strs);
  int i = transExp(exp->binop.right, NULL, vars, strs);
  if (i == -1) {
    removeTemp(t1);
    return emitOp(exp->binop.op, dest, t1, NULL, (int)exp->binop.right->val);
  }
  char *t2 = newTemp();
  transExp(exp->binop.right, t2, vars, strs);
  removeTemp(t1);
  removeTemp(t2);

  return emitOp(exp->binop.op, dest, t1, t2, 0);
}

void printInstructions(InstrList *list) {
  printf("\nIR:\n");
  InstrList *current = list;
  while (current != NULL) {
    switch (current->instr.opcode) {
    case SUB:
      printf("SUB %s %s %s\n", current->instr.arg1, current->instr.arg2,
             current->instr.arg3);
      break;
    case SUBI:
      printf("ADDI %s %s -%d\n", current->instr.arg1, current->instr.arg2,
             current->instr.num);
      break;
    case MULTI:
      printf("MULTI %s %s %d\n", current->instr.arg1, current->instr.arg2,
             current->instr.num);
      break;
    case MULT:
      printf("MULT %s %s %s\n", current->instr.arg1, current->instr.arg2,
             current->instr.arg3);
      break;
    case DIVIDEI:
      printf("DIVIDEI %s %s %d\n", current->instr.arg1, current->instr.arg2,
             current->instr.num);
      break;
    case DIVIDE:
      printf("DIVIDE %s %s %s\n", current->instr.arg1, current->instr.arg2,
             current->instr.arg3);
      break;
    case ADD:
      printf("ADD %s %s %s\n", current->instr.arg1, current->instr.arg2,
             current->instr.arg3);
      break;
    case ADDI:
      printf("ADDI %s %s %d\n", current->instr.arg1, current->instr.arg2,
             current->instr.num);
      break;
    case LOADADRESS:
      printf("LOADADRESS %s %s\n", current->instr.arg1, current->instr.arg2);
      break;
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
    case POWER:
      printf("POW %s %s %s\n", current->instr.arg1, current->instr.arg2,
             current->instr.arg3);
      break;
    case POWERI:
      printf("POW %s %s %d\n", current->instr.arg1, current->instr.arg2,
             current->instr.num);
      break;
      char *opStr;
      switch (current->instr.binop) {
      case XOR:
        asprintf(&opStr, "XOR %s %s %s\n", current->instr.arg1,
                 current->instr.arg2, current->instr.arg3);
        break;
      case NOT:
        asprintf(&opStr, "NOT %s %s %s\n", current->instr.arg1,
                 current->instr.arg2, current->instr.arg3);
        break;
      case POW:
        asprintf(&opStr, "POW %s %s %s\n", current->instr.arg1,
                 current->instr.arg2, current->instr.arg3);
        break;
      case PLUS:
        asprintf(&opStr, "PLUS %s %s %s\n", current->instr.arg1,
                 current->instr.arg2, current->instr.arg3);
        break;
      case MINUS:
        asprintf(&opStr, "MINUS %s %s %s\n", current->instr.arg1,
                 current->instr.arg2, current->instr.arg3);
        break;
      case TIMES:
        asprintf(&opStr, "TIMES %s %s %s\n", current->instr.arg1,
                 current->instr.arg2, current->instr.arg3);
        break;
      case DIV:
        asprintf(&opStr, "DIV %s %s %s\n", current->instr.arg1,
                 current->instr.arg2, current->instr.arg3);
        break;
      case AND:
        asprintf(&opStr, "AND %s %s %s\n", current->instr.arg1,
                 current->instr.arg2, current->instr.arg3);
        break;
      case OR:
        asprintf(&opStr, "OR %s %s %s\n", current->instr.arg1,
                 current->instr.arg2, current->instr.arg3);
        break;
      case EQ:
        asprintf(&opStr, "EQ %s %s %s\n", current->instr.arg1,
                 current->instr.arg2, current->instr.arg3);
        break;
      case NEQ:
        asprintf(&opStr, "NEQ %s %s %s\n", current->instr.arg1,
                 current->instr.arg2, current->instr.arg3);
        break;
      case LT:
        asprintf(&opStr, "LT %s %s %s\n", current->instr.arg1,
                 current->instr.arg2, current->instr.arg3);
        break;
      case GT:
        asprintf(&opStr, "GT %s %s %s\n", current->instr.arg1,
                 current->instr.arg2, current->instr.arg3);
        break;
      case LE:
        asprintf(&opStr, "LE %s %s %s\n", current->instr.arg1,
                 current->instr.arg2, current->instr.arg3);
        break;
      case GE:
        asprintf(&opStr, "GE %s %s %s\n", current->instr.arg1,
                 current->instr.arg2, current->instr.arg3);
        break;
        printf("%s", opStr);
        break;
      }
    case LABEL:
      printf("%s:\n", current->instr.arg1);
      break;
    case JUMP:
      printf("JUMP %s\n", current->instr.arg1);
      break;
    case COND: {
      char *opStr;
      switch (current->instr.binop) {
      case EQ:
        opStr = "=";
        break;
      case NEQ:
        opStr = "/=";
        break;
      case LT:
        opStr = "<";
        break;
      case GT:
        opStr = ">";
        break;
      case LE:
        opStr = "<=";
        break;
      case GE:
        opStr = ">=";
        break;
      default:
        opStr = "?";
      }
      printf("COND %s %s %s %s %s\n", current->instr.arg1, opStr,
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
