#include "ir.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int tempCount = 0;
static int labelCount = 0;
static int staticStrCount = 0;
static instrList *codeList = NULL;
static instrList *lastInstr = NULL;
static char *condLeft = NULL;
static char *condRight = NULL;

static int used[18];

int emit2(Opcode opc, char *arg1, char *arg2);
int emitUnary(Opcode opc, char *arg1);
int emit3(Opcode opc, char *arg1, char *arg2, char *arg3);
int emitMovel(char *dest, int num);
int emitOp(op op, char *dest, char *src1, char *src2, int val);
int emitCond(op op, char *src1, char *src2, char *label1, char *label2,
             int val);
int emitLabel(char *label);
int emitJump(char *label);
int emitCompoundIf(Exp exp, char *labelTrue, char *labelFalse, vars *vars,
                   stringLiterals **strs);
int emitCompoundWhile(Exp exp, char *labelTrue, char *labelFalse, vars *vars,
                      stringLiterals **strs);
int emitCompoundWhileAnd(Exp exp, char *labelTrue, char *labelFalse, vars *vars,
                         stringLiterals **strs);
int emitCompoundWhileOr(Exp exp, char *labelTrue, char *labelFalse, vars *vars,
                        stringLiterals **strs);
int emitCompoundIfAnd(Exp exp, char *labelTrue, char *labelFalse, vars *vars,
                      stringLiterals **strs);
int emitCompoundIfOr(Exp exp, char *labelTrue, char *labelFalse, vars *vars,
                     stringLiterals **strs);
int emitCompoundIfOrEx(Exp exp, char *labelTrue, char *labelFalse, vars *vars,
                       stringLiterals **strs, int *neg);
int emitCompoundIfAndEx(Exp exp, char *labelTrue, char *labelFalse, vars *vars,
                        stringLiterals **strs, int *neg);
int emitFunction(char *id, char *temp, char *temp2);

char *newTemp();
char *newLabel();

int transStm(Stm stm, vars *, stringLiterals **strs);
int transExp(Exp exp, char *dest, vars *, stringLiterals **strs);
int transBinOp(Exp exp, char *dest, vars *, stringLiterals **strs);
Exp applyNotToExpr(Exp exp, int neg, vars *);
void freeVariables(vars **variables);

char *getVarTemp(char *id, vars *vars) {
  if (!vars || !id)
    return NULL;
  while (vars) {
    if (!strcmp(id, vars->id))
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
  while (strs->next) {
    if (!strcmp(str, strs->str)) {
      fprintf(stderr, "Strings with same id found\n");
      return 0;
    }
    strs = strs->next;
  }

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

  instruction instr = {CALL, id, temp, temp2, NULL, 0};
  instrList *newNode = malloc(sizeof(instrList));
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
int emit1(Opcode opc, int val) {
  instruction instr = {opc, NULL, NULL, NULL, NULL, val};
  instrList *newNode = malloc(sizeof(instrList));
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
  instruction instr = {opc, arg1, arg2, NULL, NULL, 0};
  instrList *newNode = malloc(sizeof(instrList));
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
  instruction instr = {opc, arg1, arg2, arg3, NULL, 0};
  instrList *newNode = malloc(sizeof(instrList));
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
int emitUnary(Opcode opc, char *arg1) {
  instruction instr = {UNARY, arg1, arg1, NULL, NULL, 0, opc};
  instrList *newNode = malloc(sizeof(instrList));
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
  instruction instr = {MOVEI, dest, NULL, NULL, NULL, num};
  instrList *newNode = malloc(sizeof(instrList));
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
  case BNEZ:
    return BNEZ;

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
    return ope;
  case XOR:
    if (immediate)
      return XRI;
    return XR;
  case EQ:
    if (immediate)
      return EQUALSI;
    return EQUALS;
  case NEQ:
    if (immediate)
      return NOTEQUALSI;
    return NOTEQUALS;
  case LT:
    if (immediate)
      return LESSERI;
    return LESSER;
  case GT:
    if (immediate)
      return GREATERI;
    return GREATER;
  case LE:
    if (immediate)
      return LESSEREQI;
    return LESSEREQ;
  case GE:
    if (immediate)
      return GREATEREQI;
    return GREATEREQ;
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
  instruction instr = {opConverted, dest, src1, src2, NULL, val, 0};
  instrList *newNode = malloc(sizeof(instrList));
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
  instruction instr = {LABEL, label, NULL, NULL, NULL, 0};
  instrList *newNode = malloc(sizeof(instrList));
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
  instruction instr = {JUMP, label, NULL, NULL, NULL, 0};
  instrList *newNode = malloc(sizeof(instrList));
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

int emitCond(op op, char *src1, char *src2, char *label1, char *label2,
             int val) {
  Opcode opConverted = convertOp(op, src2 == NULL);

  instruction instr = {opConverted, src1, src2, label1, label2, val, 0};
  instrList *newNode = malloc(sizeof(instrList));
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

vars *addNode(char *id, char *temp, vars *vars) {
  if (!vars) {
    vars = (struct _variables *)malloc(sizeof(struct _variables));
    vars->next = NULL;
    vars->id = strdup(id);
    if (!temp)
      return NULL;

    else
      vars->temp = strdup(temp);
    return vars;
  }
  struct _variables *head = vars;
  while (vars->next)
    vars = vars->next;

  vars->next = (struct _variables *)malloc(sizeof(struct _variables));
  vars->next->next = NULL;

  vars->next->id = strdup(id);
  if (!temp)
    return NULL;
  else
    vars->next->temp = strdup(temp);
  return head;
}
vars *transVarDecl(Stm varDecl, vars *vars, stringLiterals **strs, int *error) {
  if (*error)
    return NULL;
  if (!varDecl)
    return vars;
  int ret = 1;
  if (varDecl->compound.fst->assign.type == NUM) {

    char *id = strdup(varDecl->compound.fst->assign.ident);
    char *temp = newTemp();
    vars = addNode(id, temp, vars);
    if (!vars) {
      fprintf(stderr, "Error while addind node to vars struct\n");
      *error = 1;
      return NULL;
    }
    ret = transExp(varDecl->compound.fst->assign.expr, temp, vars, strs);
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
    ret = transExp(varDecl->compound.fst->assign.expr, temp, vars, strs);
  }
  if (varDecl->compound.fst->assign.type == STRLITERAL) {

    char *id = strdup(varDecl->compound.fst->assign.ident);
    char *temp = newTemp();
    vars = addNode(id, temp, vars);
    if (!vars) {
      fprintf(stderr, "Error while addind node to vars struct\n");
      *error = 1;
      return NULL;
    }
    *strs = addString(id, varDecl->compound.fst->assign.expr->str, *strs);
    ret = (*strs) != NULL;
    emit2(LOADADRESS, temp, id);
    // transExp(varDecl->compound.fst->assign.expr, vars->temp, vars, strs);
  }
  if (!ret)
    return NULL;
  return transVarDecl(varDecl->compound.snd, vars, strs, error);
}
void printVars(vars *vars) {
  struct _variables *head = vars;
  while (head) {

    printf("%s %s\n", head->id, head->temp);
    head = head->next;
  }
}
instrList *generateIR(Prog program, stringLiterals **strs) {

  staticStrCount = 0;
  tempCount = 0;
  labelCount = 0;
  codeList = NULL;
  lastInstr = NULL;
  int error = 0;
  stringLiterals *strsLocal = NULL;
  strsLocal =
      addString("askInputStr", "\"Enter string (max 64 chars): \"", *strs);
  if (!strsLocal) {
    fprintf(stderr, "Unable to add static string to stringLiterals struct\n");
    return 0;
  }

  for (int i = 0; i < 18; i++)
    used[i] = 0;

  vars *vars = transVarDecl(program->varDec, NULL, &strsLocal, &error);
  if (error) {
    freeVariables(&vars);
    return NULL;
  }
  // printVars(vars);

  if (!transStm(program->statements, vars, &strsLocal))
    return 0;

  *strs = strsLocal;
  freeVariables(&vars);

  return codeList;
}
Exp applyNotToExpr(Exp exp, int neg, vars *vars) {
  if (!exp) {
    fprintf(stderr, "Expression applied Not was null\n");
    return NULL;
  } else if (exp->tag == UNARYOP && exp->unaryop.op == NOT) {
    if (!neg)
      return applyNotToExpr(exp->unaryop.exp, 1, vars);
    return applyNotToExpr(exp->unaryop.exp, 0, vars);
  }
  switch (exp->tag) {
  case NUM:
    if ((int)exp->val)
      exp->val = 0;
    else
      exp->val = 1;
    return exp;
    break;
  case BOOL:
    if (exp->bool_val)
      exp->bool_val = 0;
    else
      exp->bool_val = 1;
    return exp;
  case ID: {
    if (neg) {
      char *id = getVarTemp(exp->id, vars);
      if (!id) {
        fprintf(stderr, "Unable to find register binded to variable\n");
        return NULL;
      }
      id = strdup(id);
      emit2(NEG, id, id);
    }
    return exp;

  } break;
  default:
    break;
  }

  if (exp->tag == BINOP) {

    switch (exp->binop.op) {
    case EQ:
      if (neg)
        return mkBinOp(exp->binop.left, NEQ, exp->binop.right);
      return exp;
      break;
    case NEQ:
      if (neg)
        return mkBinOp(exp->binop.left, EQ, exp->binop.right);
      return exp;
      break;
    case GT:
      if (neg)
        return mkBinOp(exp->binop.left, LE, exp->binop.right);
      return exp;
      break;
    case GE:
      if (neg)
        return mkBinOp(exp->binop.left, LT, exp->binop.right);
      return exp;
      break;
    case LT:
      if (neg)
        return mkBinOp(exp->binop.left, GE, exp->binop.right);
      return exp;
      break;
    case LE:
      if (neg)
        return mkBinOp(exp->binop.left, GT, exp->binop.right);
      return exp;
      break;
    case AND:
      if (neg)
        return mkBinOp(applyNotToExpr(exp->binop.left, neg, vars), OR,
                       applyNotToExpr(exp->binop.right, neg, vars));
      return exp;
      break;
    case OR:
      if (neg)
        return mkBinOp(applyNotToExpr(exp->binop.left, neg, vars), AND,
                       applyNotToExpr(exp->binop.right, neg, vars));
      return exp;
      break;
    }
  }

  return exp;
}
int emitCompoundWhile(Exp exp, char *labelTrue, char *labelFalse, vars *vars,
                      stringLiterals **strs) {
  if (exp->tag == UNARYOP && exp->unaryop.op == NOT) {

    exp = applyNotToExpr(exp->unaryop.exp, 1, vars);
    if (!exp) {
      fprintf(stderr, "Expression returned from applying Not is NULL\n");
      return 0;
    }
  }

  if (exp->tag == BINOP && exp->binop.op == AND)
    return emitCompoundWhileAnd(exp, labelTrue, labelFalse, vars, strs);
  else if (exp->tag == BINOP && exp->binop.op == OR)
    return emitCompoundWhileOr(exp, labelTrue, labelFalse, vars, strs);
  else if (exp->tag == BINOP) {
    int ret = transExp(exp, NULL, vars, strs);
    int val = 0;
    if (ret == -1) {
      ret = 1;
      val = exp->binop.right->val;
    }
    return ret &&
           emitCond(exp->binop.op, condLeft, condRight, labelTrue, labelFalse,
                    val) &&
           emitJump(labelFalse);
  } else if (exp->tag == ID) {
    char *id = getVarTemp(exp->id, vars);
    if (id == NULL) {
      fprintf(stderr, "Unable to find register for variable %s\n", exp->id);
      return 0;
    }
    id = strdup(id);
    return transExp(exp, id, vars, strs) &&
           emitCond(BNEZ, id, NULL, labelTrue, labelFalse, 0) &&
           emitJump(labelFalse);
  } else if (exp->tag == BOOL || exp->tag == NUM) {
    char *temp = newTemp();
    int ret = transExp(exp, temp, vars, strs) &&
              emitCond(BNEZ, temp, NULL, labelTrue, labelFalse, 0) &&
              emitJump(labelFalse);
    removeTemp(temp);
    return ret;
  }
  return 0;
}
int emitCompoundIfEx(Exp exp, char *labelTrue, char *labelFalse, vars *vars,
                     stringLiterals **strs, int *neg) {
  while (exp->tag == UNARYOP && exp->unaryop.op == NOT) {
    exp = exp->unaryop.exp;
    *neg = !(*neg);
    char *temp = strdup(labelTrue);
    labelTrue = strdup(labelFalse);
    labelFalse = strdup(temp);
  }

  if (exp->tag == BINOP && exp->binop.op == AND)
    return emitCompoundIfAndEx(exp, labelTrue, labelFalse, vars, strs, neg);
  else if (exp->tag == BINOP && exp->binop.op == OR)
    return emitCompoundIfOrEx(exp, labelTrue, labelFalse, vars, strs, neg);
  else if (exp->tag == BINOP) {
    int ret = transExp(exp, NULL, vars, strs);
    int val = 0;
    if (ret == -1) {
      ret = 1;
      val = exp->binop.right->val;
    }
    return ret && emitCond(exp->binop.op, condLeft, condRight, labelTrue,
                           labelFalse, val);
  } else if (exp->tag == ID) {
    char *id = getVarTemp(exp->id, vars);
    if (id == NULL) {
      fprintf(stderr, "Unable to find register for variable %s\n", exp->id);
      return 0;
    }
    id = strdup(id);
    return transExp(exp, id, vars, strs) &&
           emitCond(NEQ, id, "zero", labelTrue, labelFalse, 0);
  } else if (exp->tag == BOOL || exp->tag == NUM) {
    char *temp = newTemp();
    int ret = transExp(exp, temp, vars, strs) &&
              emitCond(NEQ, temp, "zero", labelTrue, labelFalse, 0);
    removeTemp(temp);
    return ret;
  }
  return 0;
}
int emitCompoundIf(Exp exp, char *labelTrue, char *labelFalse, vars *vars,
                   stringLiterals **strs) {
  int neg = 0;
  if (exp->tag == UNARYOP && exp->unaryop.op == NOT) {

    exp = applyNotToExpr(exp->unaryop.exp, 1, vars);
    if (!exp) {
      fprintf(stderr, "Expression returned after applying Not is NULL\n");
      return 0;
    }
  }

  if (exp->tag == BINOP && exp->binop.op == AND)
    return emitCompoundIfAnd(exp, labelTrue, labelFalse, vars, strs);
  else if (exp->tag == BINOP && exp->binop.op == OR)
    return emitCompoundIfOr(exp, labelTrue, labelFalse, vars, strs);
  else if (exp->tag == BINOP) {
    int ret = transExp(exp, NULL, vars, strs);
    int val = 0;
    if (ret == -1) {
      ret = 1;
      val = exp->binop.right->val;
    }
    return ret && emitCond(exp->binop.op, condLeft, condRight, labelTrue,
                           labelFalse, val);
  } else if (exp->tag == ID) {
    char *id = getVarTemp(exp->id, vars);
    if (id == NULL) {
      fprintf(stderr, "Unable to find register for variable %s\n", exp->id);
      return 0;
    }
    id = strdup(id);
    return transExp(exp, id, vars, strs) &&
           emitCond(NEQ, id, "zero", labelTrue, labelFalse, 0);
  } else if (exp->tag == BOOL || exp->tag == NUM) {
    char *temp = newTemp();
    int ret = transExp(exp, temp, vars, strs) &&
              emitCond(NEQ, temp, "zero", labelTrue, labelFalse, 0);
    removeTemp(temp);
    return ret;
  }
  return 0;
}
int emitCompoundWhileAnd(Exp exp, char *labelTrue, char *labelFalse, vars *vars,
                         stringLiterals **strs) {
  if (exp->tag == BINOP && exp->binop.op == AND) {
    if (exp->binop.left->tag == UNARYOP && exp->binop.left->unaryop.op == NOT) {

      exp->binop.left = applyNotToExpr(exp->binop.left->unaryop.exp, 1, vars);
      if (!exp->binop.left) {
        fprintf(stderr, "Expression returned after applying Not is NULL\n");
        return 0;
      }
    }

    if (exp->binop.left->tag == BINOP && exp->binop.left->binop.op == AND)
      return emitCompoundWhileAnd(exp->binop.left, labelTrue, labelFalse, vars,
                                  strs) &&
             emitCompoundWhileAnd(exp->binop.right, labelTrue, labelFalse, vars,
                                  strs);
    else if (exp->binop.left->tag == BINOP && exp->binop.left->binop.op == OR)
      return emitCompoundWhileOr(exp->binop.left, labelTrue, labelFalse, vars,
                                 strs) &&
             emitCompoundWhileAnd(exp->binop.right, labelTrue, labelFalse, vars,
                                  strs);

    char *labelAnd = newLabel();
    int ret = transExp(exp->binop.left, NULL, vars, strs);
    int val = 0;
    if (ret == -1) {
      ret = 1;
      val = exp->binop.left->binop.right->val;
    }
    return ret &&
           emitCond(exp->binop.left->binop.op, condLeft, condRight, labelAnd,
                    labelFalse, val) &&
           emitJump(labelFalse) && emitLabel(labelAnd) &&
           emitCompoundWhileAnd(exp->binop.right, labelTrue, labelFalse, vars,
                                strs);
  }
  if (exp->tag == BINOP && exp->binop.op != OR) {
    int ret = transExp(exp, NULL, vars, strs);
    int val = 0;
    if (ret == -1) {
      ret = 1;
      val = exp->binop.right->val;
    }
    return ret &&
           emitCond(exp->binop.op, condLeft, condRight, labelTrue, labelFalse,
                    val) &&

           emitJump(labelFalse);
  }
  return emitCompoundWhile(exp, labelTrue, labelFalse, vars, strs);
  return 0;
}
int emitCompoundWhileOr(Exp exp, char *labelTrue, char *labelFalse, vars *vars,
                        stringLiterals **strs) {
  if (exp->tag == BINOP && exp->binop.op == OR) {
    if (exp->binop.left->tag == UNARYOP && exp->binop.left->unaryop.op == NOT) {

      exp->binop.left = applyNotToExpr(exp->binop.left->unaryop.exp, 1, vars);
      if (!exp->binop.left) {

        fprintf(stderr, "Expression returned after applying Not is NULL\n");
        return 0;
      }
    }

    if (exp->binop.left->tag == BINOP && exp->binop.left->binop.op == AND)
      return emitCompoundWhileAnd(exp->binop.left, labelTrue, labelFalse, vars,
                                  strs) &&
             emitCompoundWhileOr(exp->binop.right, labelTrue, labelFalse, vars,
                                 strs);
    else if (exp->binop.left->tag == BINOP && exp->binop.left->binop.op == OR)
      return emitCompoundWhileOr(exp->binop.left, labelTrue, labelFalse, vars,
                                 strs) &&
             emitCompoundWhileOr(exp->binop.right, labelTrue, labelFalse, vars,
                                 strs);

    int ret = transExp(exp->binop.left, NULL, vars, strs);
    int val = 0;
    if (ret == -1) {
      ret = 1;
      val = exp->binop.left->binop.right->val;
    }
    return ret &&
           emitCond(exp->binop.left->binop.op, condLeft, condRight, labelTrue,
                    labelFalse, val) &&
           emitJump(labelFalse) &&
           emitCompoundWhileOr(exp->binop.right, labelTrue, labelFalse, vars,
                               strs);
  }
  if (exp->tag == BINOP && exp->binop.op != AND) {
    int ret = transExp(exp, NULL, vars, strs);
    int val = 0;
    if (ret == -1) {
      ret = 1;
      val = exp->binop.right->val;
    }
    return ret &&
           emitCond(exp->binop.op, condLeft, condRight, labelTrue, labelFalse,
                    val) &&
           emitJump(labelFalse);
  }
  return emitCompoundWhile(exp, labelTrue, labelFalse, vars, strs);
  return 0;
}

int emitCompoundIfOrEx(Exp exp, char *labelTrue, char *labelFalse, vars *vars,
                       stringLiterals **strs, int *neg) {
  while (exp->tag == UNARYOP && exp->unaryop.op == NOT) {
    exp = exp->unaryop.exp;
    *neg = !(*neg);
    char *temp = strdup(labelTrue);
    labelTrue = strdup(labelFalse);
    labelFalse = strdup(temp);
  }
  while (exp->tag == BINOP && exp->binop.left->tag == UNARYOP &&
         exp->binop.left->unaryop.op == NOT) {
    exp->binop.left = exp->binop.left->unaryop.exp;
    *neg = !(*neg);
    char *temp = strdup(labelTrue);
    labelTrue = strdup(labelFalse);
    labelFalse = strdup(temp);
  }
  if (exp->tag == BINOP && exp->binop.op == OR) {

    if (exp->binop.left->tag == BINOP && exp->binop.left->binop.op == AND)
      return emitCompoundIfAndEx(exp->binop.left, labelTrue, labelFalse, vars,
                                 strs, neg) &&
             emitCompoundIfOrEx(exp->binop.right, labelTrue, labelFalse, vars,
                                strs, neg);
    else if (exp->binop.left->tag == BINOP && exp->binop.left->binop.op == OR)
      return emitCompoundIfOrEx(exp->binop.left, labelTrue, labelFalse, vars,
                                strs, neg) &&
             emitCompoundIfOrEx(exp->binop.right, labelTrue, labelFalse, vars,
                                strs, neg);

    char *labelOr = newLabel();
    if (exp->binop.left->tag == BOOL || exp->binop.left->tag == NUM) {
      char *temp = newTemp();
      int ret = transExp(exp->binop.left, temp, vars, strs) &&
                emitCond(BNEZ, temp, NULL, labelTrue, labelOr, 0) &&
                emitLabel(labelOr);
      removeTemp(temp);
      return ret && emitCompoundIfOrEx(exp->binop.right, labelTrue, labelFalse,
                                       vars, strs, neg);
    }

    if (exp->binop.left->tag == ID) {
      char *id = getVarTemp(exp->binop.left->id, vars);
      if (id == NULL) {
        fprintf(stderr, "Unable to find register for variable %s\n",
                exp->binop.left->id);
        return 0;
      }
      id = strdup(id);
      return transExp(exp->binop.left, id, vars, strs) &&
             emitCond(BNEZ, id, NULL, labelTrue, labelOr, 0) &&
             emitLabel(labelOr) &&
             emitCompoundIfOrEx(exp->binop.right, labelTrue, labelFalse, vars,
                                strs, neg);
    }
    int ret = transExp(exp->binop.left, NULL, vars, strs);
    int val = 0;
    if (ret == -1) {
      ret = 1;
      val = exp->binop.left->binop.right->val;
    }
    return ret &&
           emitCond(exp->binop.left->binop.op, condLeft, condRight, labelTrue,
                    labelOr, val) &&
           emitLabel(labelOr) &&
           emitCompoundIfOrEx(exp->binop.right, labelTrue, labelFalse, vars,
                              strs, neg);
  }
  if (exp->tag == BINOP && exp->binop.op != AND) {
    int ret = transExp(exp, NULL, vars, strs);
    int val = 0;
    if (ret == -1) {
      ret = 1;
      val = exp->binop.right->val;
    }
    return ret && emitCond(exp->binop.op, condLeft, condRight, labelTrue,
                           labelFalse, val);
  }
  return emitJump(labelTrue) &&
         emitCompoundIfEx(exp, labelTrue, labelFalse, vars, strs, neg);
}
int emitCompoundIfAndEx(Exp exp, char *labelTrue, char *labelFalse, vars *vars,
                        stringLiterals **strs, int *neg) {

  while (exp->tag == UNARYOP && exp->unaryop.op == NOT) {
    exp = exp->unaryop.exp;
    *neg = !(*neg);
    char *temp = strdup(labelTrue);
    labelTrue = strdup(labelFalse);
    labelFalse = strdup(temp);
  }
  while (exp->tag == BINOP && exp->binop.left->tag == UNARYOP &&
         exp->binop.left->unaryop.op == NOT) {
    exp->binop.left = exp->binop.left->unaryop.exp;
    *neg = !(*neg);
    char *temp = strdup(labelTrue);
    labelTrue = strdup(labelFalse);
    labelFalse = strdup(temp);
  }
  if (exp->tag == BINOP && exp->binop.op == AND) {

    if (exp->binop.left->tag == BINOP && exp->binop.left->binop.op == AND)
      return emitCompoundIfAndEx(exp->binop.left, labelTrue, labelFalse, vars,
                                 strs, neg) &&
             emitCompoundIfAndEx(exp->binop.right, labelTrue, labelFalse, vars,
                                 strs, neg);
    else if (exp->binop.left->tag == BINOP && exp->binop.left->binop.op == OR)
      return emitCompoundIfOrEx(exp->binop.left, labelTrue, labelFalse, vars,
                                strs, neg) &&
             emitCompoundIfAndEx(exp->binop.right, labelTrue, labelFalse, vars,
                                 strs, neg);

    char *id = NULL;
    char *labelAnd = newLabel();
    if (exp->binop.left->tag == BOOL || exp->binop.left->tag == NUM) {
      id = newTemp();
      int ret = transExp(exp->binop.left, id, vars, strs) &&
                emitCond(BNEZ, id, NULL, labelAnd, labelFalse, 0) &&
                emitJump(labelFalse) && emitLabel(labelAnd);
      removeTemp(id);
      return ret && emitCompoundIfAndEx(exp->binop.right, labelTrue, labelFalse,
                                        vars, strs, neg);
    } else if (exp->binop.left->tag == ID) {
      char *id = getVarTemp(exp->binop.left->id, vars);
      if (!id) {
        fprintf(stderr, "Unable to find register for variable %s\n",
                exp->binop.left->id);
        return 0;
      }
      id = strdup(id);
      return transExp(exp->binop.left, id, vars, strs) &&
             emitCond(BNEZ, id, NULL, labelAnd, labelFalse, 0) &&
             emitJump(labelFalse) && emitLabel(labelAnd) &&
             emitCompoundIfAndEx(exp->binop.right, labelTrue, labelFalse, vars,
                                 strs, neg);
    }
    int ret = transExp(exp->binop.left, NULL, vars, strs);
    int val = 0;
    if (ret == -1) {
      ret = 1;
      val = exp->binop.left->binop.right->val;
    }
    ret = ret &&
          emitCond(exp->binop.left->binop.op, condLeft, condRight, labelAnd,
                   labelFalse, val) &&
          emitLabel(labelAnd) &&
          emitCompoundIfAndEx(exp->binop.right, labelTrue, labelFalse, vars,
                              strs, neg);
    return ret;
  } else if (exp->tag == BINOP && exp->binop.op != OR) {
    int ret = transExp(exp, NULL, vars, strs);
    int val = 0;
    if (ret == -1) {
      ret = 1;
      val = exp->binop.right->val;
    }
    return ret && emitCond(exp->binop.op, condLeft, condRight, labelTrue,
                           labelFalse, val);
  }
  return emitCompoundIfEx(exp, labelTrue, labelFalse, vars, strs, neg);
}
int emitCompoundIfAnd(Exp exp, char *labelTrue, char *labelFalse, vars *vars,
                      stringLiterals **strs) {
  if (exp->tag == BINOP && exp->binop.op == AND) {
    if (exp->binop.left->tag == UNARYOP && exp->binop.left->unaryop.op == NOT) {

      exp->binop.left = applyNotToExpr(exp->binop.left->unaryop.exp, 1, vars);
      if (!exp->binop.left) {
        fprintf(stderr, "Expression returned after applying Not is NULL\n");
        return 0;
      }
    }

    if (exp->binop.left->tag == BINOP && exp->binop.left->binop.op == AND)
      return emitCompoundIfAnd(exp->binop.left, labelTrue, labelFalse, vars,
                               strs) &&
             emitCompoundIfAnd(exp->binop.right, labelTrue, labelFalse, vars,
                               strs);
    else if (exp->binop.left->tag == BINOP && exp->binop.left->binop.op == OR)
      return emitCompoundIfOr(exp->binop.left, labelTrue, labelFalse, vars,
                              strs) &&
             emitCompoundIfAnd(exp->binop.right, labelTrue, labelFalse, vars,
                               strs);

    char *labelAndTrue = newLabel();
    char *id = NULL;
    if (exp->binop.left->tag == BOOL || exp->binop.left->tag == NUM) {
      id = newTemp();
      int ret = transExp(exp->binop.left, id, vars, strs) &&
                emitCond(NEQ, id, "zero", labelAndTrue, labelFalse, 0) &&
                emitLabel(labelAndTrue);
      removeTemp(id);
      return ret && emitCompoundIf(exp->binop.right, labelTrue, labelFalse,
                                   vars, strs);
    } else if (exp->binop.left->tag == ID) {
      char *id = getVarTemp(exp->binop.left->id, vars);
      if (!id) {
        fprintf(stderr, "Unable to find register for variable %s\n",
                exp->binop.left->id);
        return 0;
      }
      id = strdup(id);
      return transExp(exp->binop.left, id, vars, strs) &&
             emitCond(NEQ, id, "zero", labelAndTrue, labelFalse, 0) &&
             emitLabel(labelAndTrue) &&
             emitCompoundIf(exp->binop.right, labelTrue, labelFalse, vars,
                            strs);
    }
    int ret = transExp(exp->binop.left, NULL, vars, strs);
    int val = 0;
    if (ret == -1) {
      ret = 1;
      val = exp->binop.left->binop.right->val;
    }
    ret = ret &&
          emitCond(exp->binop.left->binop.op, condLeft, condRight, labelAndTrue,
                   labelFalse, val) &&
          emitLabel(labelAndTrue) &&
          emitCompoundIf(exp->binop.right, labelTrue, labelFalse, vars, strs);
    return ret;
  }
  if (exp->tag == BINOP && exp->binop.op != OR) {
    int ret = transExp(exp, NULL, vars, strs);
    int val = 0;
    if (ret == -1) {
      ret = 1;
      val = exp->binop.right->val;
    }
    return ret && emitCond(exp->binop.op, condLeft, condRight, labelTrue,
                           labelFalse, val);
  }
  return emitCompoundIf(exp, labelTrue, labelFalse, vars, strs);
  return 0;
}
int emitCompoundIfOr(Exp exp, char *labelTrue, char *labelFalse, vars *vars,
                     stringLiterals **strs) {
  if (exp->tag == BINOP && exp->binop.op == OR) {
    if (exp->binop.left->tag == UNARYOP && exp->binop.left->unaryop.op == NOT) {

      exp->binop.left = applyNotToExpr(exp->binop.left->unaryop.exp, 1, vars);
      if (!exp->binop.left) {

        fprintf(stderr, "Expression returned after applying Not is NULL\n");
        return 0;
      }
    }

    if (exp->binop.left->tag == BINOP && exp->binop.left->binop.op == AND)
      return emitCompoundIfAnd(exp->binop.left, labelTrue, labelFalse, vars,
                               strs) &&
             emitCompoundIfOr(exp->binop.right, labelTrue, labelFalse, vars,
                              strs);
    else if (exp->binop.left->tag == BINOP && exp->binop.left->binop.op == OR)
      return emitCompoundIfOr(exp->binop.left, labelTrue, labelFalse, vars,
                              strs) &&
             emitCompoundIfOr(exp->binop.right, labelTrue, labelFalse, vars,
                              strs);

    if (exp->binop.left->tag == BOOL || exp->binop.left->tag == NUM) {
      char *temp = newTemp();
      int ret = transExp(exp->binop.left, temp, vars, strs) &&
                emitCond(NEQ, temp, "zero", labelFalse, labelTrue, 0);
      removeTemp(temp);
      return ret && emitCompoundIf(exp->binop.right, labelTrue, labelFalse,
                                   vars, strs);
    }

    if (exp->binop.left->tag == ID) {
      char *id = getVarTemp(exp->binop.left->id, vars);
      if (id == NULL) {
        fprintf(stderr, "Unable to find register for variable %s\n",
                exp->binop.left->id);
        return 0;
      }
      id = strdup(id);
      return transExp(exp->binop.left, id, vars, strs) &&
             emitCond(NEQ, id, "zero", labelFalse, labelTrue, 0) &&
             emitCompoundIf(exp->binop.right, labelTrue, labelFalse, vars,
                            strs);
    }
    int ret = transExp(exp->binop.left, NULL, vars, strs);
    int val = 0;
    if (ret == -1) {
      ret = 1;
      val = exp->binop.left->binop.right->val;
    }
    return ret &&
           emitCond(exp->binop.left->binop.op, condLeft, condRight, labelFalse,
                    labelTrue, val) &&
           emitCompoundIfOr(exp->binop.right, labelTrue, labelFalse, vars,
                            strs);
  }
  if (exp->tag == BINOP && exp->binop.op != AND) {
    int ret = transExp(exp, NULL, vars, strs);
    int val = 0;
    if (ret == -1) {
      ret = 1;
      val = exp->binop.right->val;
    }
    return ret && emitCond(exp->binop.op, condLeft, condRight, labelFalse,
                           labelTrue, val);
  }
  return emitCompoundIf(exp, labelTrue, labelFalse, vars, strs);
  return 0;
}

int transStm(Stm stm, vars *vars, stringLiterals **strs) {
  if (!stm)
    return 0;

  switch (stm->tag) {
  case ASSIGN: {
    char *id = getVarTemp(stm->assign.ident, vars);
    if (!id) {
      fprintf(stderr, "Unable to find variable to bind the value\n");
      return 0;
    }
    id = strdup(id);
    char *temp = NULL;
    int remove = 0;
    if (stm->assign.expr->tag == ID) {

      temp = getVarTemp(stm->assign.expr->id, vars);
      if (!temp) {
        fprintf(stderr, "Unable to find variable to bind the value\n");
        return 0;
      }
      temp = strdup(temp);
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
    char *labelTrue = newLabel();
    char *labelFalse = newLabel();
    char *labelEnd = newLabel();
    char *trueLTrue = strdup(labelTrue);
    char *trueLFalse = strdup(labelFalse);
    int neg = 0;
    int ret = 0;

    while (stm->ifStmt.cond->tag == UNARYOP &&
           stm->ifStmt.cond->unaryop.op == NOT) {
      neg = !neg;
      stm->ifStmt.cond = stm->ifStmt.cond->unaryop.exp;
      char *temp = strdup(labelTrue);
      labelTrue = strdup(labelFalse);
      labelFalse = strdup(temp);
    }

    if (stm->ifStmt.cond->binop.op == AND || stm->ifStmt.cond->binop.op == OR) {

      ret = emitCompoundIfEx(stm->ifStmt.cond, labelTrue, labelFalse, vars,
                             strs, &neg);
      if (!ret) {
        fprintf(stderr,
                "Unable to generate branches for the given condition\n");
        return 0;
      }

    } else if (stm->ifStmt.cond->tag == ID) {
      char *id = getVarTemp(stm->ifStmt.cond->id, vars);
      if (id == NULL) {
        fprintf(stderr, "Unable to find register for variable %s\n",
                stm->ifStmt.cond->id);
        return 0;
      }
      id = strdup(id);
      ret = transExp(stm->ifStmt.cond, id, vars, strs);
      ret = ret && emitCond(NEQ, id, "zero", labelTrue, labelFalse, 0);

    } else if (stm->ifStmt.cond->tag == BOOL || stm->ifStmt.cond->tag == NUM) {
      temp = newTemp();
      ret = transExp(stm->ifStmt.cond, temp, vars, strs);
      ret = ret && emitCond(NEQ, temp, "zero", labelTrue, labelFalse, 0);
      removeTemp(temp);

    } else {

      ret = transExp(stm->ifStmt.cond, temp, vars, strs);
      ret = ret && emitCond(stm->ifStmt.cond->binop.op, condLeft, condRight,
                            labelTrue, labelFalse, 0);
    }

    if (!neg) {

      ret = ret && emitLabel(trueLFalse);
      if (stm->ifStmt.elseBranch != NULL)
        ret = ret && transStm(stm->ifStmt.elseBranch, vars, strs) &&
              emitJump(labelEnd);

      ret = ret && emitJump(labelEnd) && emitLabel(trueLTrue) &&
            transStm(stm->ifStmt.thenBranch, vars, strs) && emitJump(labelEnd);

    } else {
      ret = ret && emitLabel(trueLTrue) &&
            transStm(stm->ifStmt.thenBranch, vars, strs) &&
            emitJump(labelEnd) && emitLabel(trueLFalse);
      if (stm->ifStmt.elseBranch != NULL)
        ret = ret && transStm(stm->ifStmt.elseBranch, vars, strs);
    }

    return ret && emitJump(labelEnd) && emitLabel(labelEnd);

    break;
  }
  case WHILE: {
    char *temp = NULL;
    char *labelFalse = newLabel();
    char *labelEnd = newLabel();
    char *labelBody = newLabel();
    int neg = 0;
    emitLabel(labelFalse);

    if (stm->whileStmt.cond->tag == UNARYOP &&
        stm->whileStmt.cond->unaryop.op == NOT)
      stm->whileStmt.cond =
          applyNotToExpr(stm->whileStmt.cond->unaryop.exp, 1, vars);

    if (stm->whileStmt.cond->binop.op == AND ||
        stm->whileStmt.cond->binop.op == OR) {

      int ret = emitCompoundWhile(stm->whileStmt.cond, labelBody, labelEnd,
                                  vars, strs);
      if (!ret) {
        fprintf(stderr,
                "Unable to generate branches for the given condition\n");
        return 0;
      }
      return ret && emitLabel(labelBody) &&
             transStm(stm->whileStmt.body, vars, strs) &&
             emitJump(labelFalse) && emitLabel(labelEnd);
    }
    if (stm->whileStmt.cond->tag == ID) {
      char *id = getVarTemp(stm->whileStmt.cond->id, vars);
      if (id == NULL) {
        fprintf(stderr, "Unable to find register for variable %s\n",
                stm->whileStmt.cond->id);
        return 0;
      }
      id = strdup(id);
      int ret = transExp(stm->whileStmt.cond, id, vars, strs) &&
                emitCond(BNEZ, id, NULL, labelBody, labelEnd, 0) &&
                emitJump(labelEnd) && emitLabel(labelBody) &&
                transStm(stm->whileStmt.body, vars, strs) &&
                emitJump(labelFalse);

      return ret && emitLabel(labelEnd);
    }
    if (stm->whileStmt.cond->tag == BOOL || stm->whileStmt.cond->tag == NUM) {
      temp = newTemp();
      int ret = transExp(stm->whileStmt.cond, temp, vars, strs) &&
                emitCond(BNEZ, temp, NULL, labelBody, labelEnd, 0) &&
                emitJump(labelEnd) && emitLabel(labelBody) &&
                transStm(stm->whileStmt.body, vars, strs) &&
                emitJump(labelFalse);
      removeTemp(temp);

      return ret && emitLabel(labelEnd);
    }

    int ret = transExp(stm->whileStmt.cond, temp, vars, strs) &&
              emitCond(stm->whileStmt.cond->binop.op, condLeft, condRight,
                       labelBody, labelEnd, 0) &&
              emitJump(labelEnd) && emitLabel(labelBody) &&

              transStm(stm->whileStmt.body, vars, strs) && emitJump(labelFalse);

    return ret &&

           emitLabel(labelEnd);
  } break;
  case INCR: {

    char *temp1 = newTemp();
    char *temp2 = newTemp();
    return emit2(MOVE, temp1, stm->ident) && emitMoveI(temp2, 1) &&
           emitOp(PLUS, stm->ident, temp1, temp2, 0);
    break;
  }

  case FUNCTION: {

    int tag = stm->function.args->arg->tag;

    char *dst1 = "a0";
    char *dst2 = NULL;
    char *temp = NULL;
    char *id1 = NULL;
    char *id2 = NULL;
    int remove = 0;
    int gLine = 0;

    if (stm->function.args->arg->tag == ID) {

      if (searchStrLiteralById(stm->function.args->arg->id, *strs)) {

        id1 = getVarTemp(stm->function.args->arg->id, vars);
        if (!id1) {
          fprintf(stderr, "Unable to find register binded by variable\n");
          return 0;
        }
        id1 = strdup(id1);
        temp = id1;
      } else {
        temp = newTemp();
        remove = 1;
      }
    } else {
      temp = newTemp();
      remove = 1;
    }
    int ret = transExp(stm->function.args->arg, temp, vars, strs);

    if (!ret) {
      fprintf(stderr, "Unable to translate expression inside function\n");
      return 0;
    }

    ret = ret && emit2(MOVE, dst1, temp);

    if (stm->function.args->nextArg) {
      if (stm->function.args->nextArg->arg->tag == ID) {
        id2 = getVarTemp(stm->function.args->nextArg->arg->id, vars);
        if (!id2) {
          fprintf(stderr, "Unable to find register binded to variable\n");
          return 0;
        }
        id2 = strdup(id2);
        temp = id2;
      } else if (!remove) {
        temp = newTemp();
        remove = 1;
      }
      dst2 = "a1";

      ret = ret && transExp(stm->function.args->nextArg->arg, temp, vars, strs);
      if (!ret) {
        fprintf(stderr, "Unable to translate expression inside function\n");
        return 0;
      }
      ret = ret && emit2(MOVE, dst2, temp);
    }

    if (remove)
      removeTemp(temp);

    if (!strcmp(stm->function.ident, "Get_Line"))
      gLine = 1;

    if (gLine)
      return ret && emit1(SAVEREGISTERS, tempCount) &&
             emitFunction(stm->function.ident, dst1, dst2) &&
             emit1(LOADREGISTERS, tempCount) && emit2(MOVE, id1, dst1) &&
             emit2(MOVE, id2, dst2);
    return ret && emit1(SAVEREGISTERS, tempCount) &&
           emitFunction(stm->function.ident, dst1, dst2) &&
           emit1(LOADREGISTERS, tempCount);
  } break;
  }
}

int transExp(Exp exp, char *dest, vars *vars, stringLiterals **strs) {
  if (!exp)
    return 0;

  switch (exp->tag) {

  case NUM:
    if (!dest)
      return -1;
    return emitMoveI(dest, (int)exp->val);
    break;
  case ID: {
    if (!dest)
      return -2;
    char *id = getVarTemp(exp->id, vars);
    if (id == NULL) {

      if (searchStrLiteralById(exp->id, *strs))
        return emit2(MOVE, dest, exp->id);
      id = exp->id;
    } else
      id = strdup(id);

    if (dest && !strcmp(dest, id))
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
      if (!id) {
        fprintf(stderr, "Unable to create static string for string literal\n");
        return 0;
      }
    } else
      id = strdup(id);
    return emit2(LOADADRESS, dest, id);
  } break;
  case UNARYOP:
    return transExp(exp->unaryop.exp, dest, vars, strs);
    break;
  }
  return 1;
}

int transBinOp(Exp exp, char *dest, vars *vars, stringLiterals **strs) {
  if (dest == NULL) {

    condLeft = NULL;
    condRight = NULL;
    int temp1 = 0, temp2 = 0;
    if (exp->binop.left->tag == ID) {
      condLeft = getVarTemp(exp->binop.left->id, vars);
      if (!condLeft) {
        fprintf(stderr, "Unable to find register binded to variable\n");
        return 0;
      }
      condLeft = strdup(condLeft);
    } else if (!condLeft) {
      condLeft = newTemp();
      temp1 = 1;
    }

    transExp(exp->binop.left, condLeft, vars, strs);
    if (exp->binop.right->tag == ID) {
      condRight = getVarTemp(exp->binop.right->id, vars);
      if (!condLeft) {
        fprintf(stderr, "Unable to find register binded to variable\n");
        return 0;
      }
      condRight = strdup(condRight);
    } else if (!condRight) {
      condRight = newTemp();
      temp2 = 1;
    }

    transExp(exp->binop.right, condRight, vars, strs);

    if (temp1)
      removeTemp(condLeft);
    if (temp2)
      removeTemp(condRight);
    return 1;
  }
  char *t1 = NULL;
  int temp1 = 0, temp2 = 0;
  if (exp->binop.left->tag == ID) {
    t1 = getVarTemp(exp->binop.left->id, vars);
    if (!t1) {
      fprintf(stderr, "Unable to find register binded to variable\n");
      return 0;
    }
    t1 = strdup(t1);
  } else if (!t1) {
    t1 = newTemp();
    temp1 = 1;
  }

  transExp(exp->binop.left, t1, vars, strs);

  int i = transExp(exp->binop.right, NULL, vars, strs);
  char *t2 = NULL;
  if (i == -1) {
    if (temp1)
      removeTemp(t1);
    return emitOp(exp->binop.op, dest, t1, NULL, (int)exp->binop.right->val);
  } else if (i == -2) {
    t2 = getVarTemp(exp->binop.right->id, vars);
    if (!t2) {
      fprintf(stderr, "Unable to find register binded to variable\n");
      return 0;
    }
    t2 = strdup(t2);
  } else if (i == 0)
    return 0;
  else if (!t2) {
    t2 = newTemp();
    temp2 = 1;
  }
  transExp(exp->binop.right, t2, vars, strs);
  if (temp1)
    removeTemp(t1);
  if (temp2)
    removeTemp(t2);

  return emitOp(exp->binop.op, dest, t1, t2, 0);
}

void printInstructions(instrList *list) {
  printf("\nIR:\n");
  instrList *current = list;
  while (current != NULL) {
    switch (current->instr.opcode) {
    case SAVEREGISTERS:
      printf("STORE %d registers\n", current->instr.num);
      break;
    case LOADREGISTERS:
      printf("LOAD %d registers\n", current->instr.num);
      break;
    case UNARY: {
      switch (current->instr.binop) {
      case NOT:
        printf("NOT %s \n", current->instr.arg1);
        break;
      }

      break;
    }
    case XR:
      printf("XOR %s %s %s\n", current->instr.arg1, current->instr.arg2,
             current->instr.arg3);
      break;
    case XRI:
      printf("XR %s %s %d\n", current->instr.arg1, current->instr.arg2,
             current->instr.num);
      break;
    case NEG:
      printf("NOT %s\n", current->instr.arg1);
      break;
    case BNEZ:
      printf("COND %s /= 0 %s %s\n", current->instr.arg1, current->instr.arg3,
             current->instr.arg4);
      break;
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
    case GREATEREQ:
      printf("COND %s >= %s %s %s\n", current->instr.arg1, current->instr.arg2,
             current->instr.arg3, current->instr.arg4);
      break;
    case LESSEREQ:
      printf("COND %s <= %s %s %s\n", current->instr.arg1, current->instr.arg2,
             current->instr.arg3, current->instr.arg4);
      break;
    case EQUALS:
      printf("COND %s == %s %s %s\n", current->instr.arg1, current->instr.arg2,
             current->instr.arg3, current->instr.arg4);
      break;
    case NOTEQUALS:
      printf("COND %s /= %s %s %s\n", current->instr.arg1, current->instr.arg2,
             current->instr.arg3, current->instr.arg4);
      break;
    case GREATER:
      printf("COND %s > %s %s %s\n", current->instr.arg1, current->instr.arg2,
             current->instr.arg3, current->instr.arg4);
      break;
    case LESSER:
      printf("COND %s < %s %s %s\n", current->instr.arg1, current->instr.arg2,
             current->instr.arg3, current->instr.arg4);
      break;
    case GREATEREQI:
      printf("COND %s >= %d %s %s\n", current->instr.arg1, current->instr.num,
             current->instr.arg3, current->instr.arg4);
      break;
    case LESSEREQI:
      printf("COND %s <= %d %s %s\n", current->instr.arg1, current->instr.num,
             current->instr.arg3, current->instr.arg4);
      break;
    case EQUALSI:
      printf("COND %s == %d %s %s\n", current->instr.arg1, current->instr.num,
             current->instr.arg3, current->instr.arg4);
      break;
    case NOTEQUALSI:
      printf("COND %s /= %d %s %s\n", current->instr.arg1, current->instr.num,
             current->instr.arg3, current->instr.arg4);
      break;
    case GREATERI:
      printf("COND %s > %d %s %s\n", current->instr.arg1, current->instr.num,
             current->instr.arg3, current->instr.arg4);
      break;
    case LESSERI:
      printf("COND %s < %d %s %s\n", current->instr.arg1, current->instr.num,
             current->instr.arg3, current->instr.arg4);
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
      case AND:
        opStr = "AND";
        break;
      case NOT:
        opStr = "NOT";
        break;
      case OR:
        opStr = "OR";
        break;
      default:
        printf("%d\n", current->instr.binop);
        opStr = "?";
      }
      opStr = "?";
      printf("COND %s %s %s %s %s\n", current->instr.arg1, opStr,
             current->instr.arg2, current->instr.arg3, current->instr.arg4);
      break;
    } break;
    }
    current = current->next;
  }
}

void freeInstructions(instrList **list) {
  while (*list) {
    instrList *next = NULL;
    if ((*list)->next)
      next = (*list)->next;

    free(*list);
    *list = next;
  }
}
void freeVariables(vars **variables) {
  while (*variables) {
    vars *next = (*variables)->next;
    free((*variables)->id);
    free((*variables)->temp);
    free(*variables);
    *variables = next;
  }
}
void freeStrings(stringLiterals **strs) {

  while (*strs) {
    stringLiterals *next = (*strs)->next;
    free((*strs)->id);
    free((*strs)->str);
    free(*strs);

    *strs = next;
  }
}
