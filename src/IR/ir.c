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

static const char *temps[18] = {"t0", "t1", "t2", "t3", "t4", "t5",
                                "t6", "t7", "t8", "t9", "s0", "s1",
                                "s2", "s3", "s4", "s5", "s6", "s7"};
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
    char *temp = id;
    vars = addNode(id, temp, vars);
    if (!vars) {
      fprintf(stderr, "Error while addind node to vars struct\n");
      *error = 1;
      return NULL;
    }
    *strs = addString(id, varDecl->compound.fst->assign.expr->str, *strs);
    ret = (*strs) != NULL;
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
  if (exp->tag == UNARYOP && exp->unaryop.op == NOT)
    exp = applyNotToExpr(exp->unaryop.exp, 1, vars);

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
    return ret && emitCond(exp->binop.op, condLeft, condRight, labelFalse,
                           labelTrue, val);
  } else if (exp->tag == ID) {
    char *id = getVarTemp(exp->id, vars);
    if (id == NULL) {
      fprintf(stderr, "Unable to find register for variable %s\n", exp->id);
      return 0;
    }
    id = strdup(id);
    return transExp(exp, id, vars, strs) &&
           emitCond(NEQ, id, "zero", labelFalse, labelTrue, 0);
  } else if (exp->tag == BOOL || exp->tag == NUM) {
    char *temp = newTemp();
    int ret = transExp(exp, temp, vars, strs) &&
              emitCond(NEQ, temp, "zero", labelFalse, labelTrue, 0);
    removeTemp(temp);
    return ret;
  }
  return 0;
}
int emitCompoundIf(Exp exp, char *labelTrue, char *labelFalse, vars *vars,
                   stringLiterals **strs) {
  int neg = 0;
  if (exp->tag == UNARYOP && exp->unaryop.op == NOT)
    exp = applyNotToExpr(exp->unaryop.exp, 1, vars);

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
    if (exp->binop.left->tag == UNARYOP && exp->binop.left->unaryop.op == NOT)
      exp->binop.left = applyNotToExpr(exp->binop.left->unaryop.exp, 1, vars);

    if (exp->binop.left->tag == BINOP && exp->binop.left->binop.op == AND)
      return emitCompoundWhileAnd(exp->binop.left, labelTrue, labelFalse, vars,
                                  strs) &&
             emitCompoundWhile(exp->binop.right, labelTrue, labelFalse, vars,
                               strs);
    else if (exp->binop.left->tag == BINOP && exp->binop.left->binop.op == OR)
      return emitCompoundWhileOr(exp->binop.left, labelTrue, labelFalse, vars,
                                 strs) &&
             emitCompoundWhile(exp->binop.right, labelTrue, labelFalse, vars,
                               strs);

    int ret = transExp(exp->binop.left, NULL, vars, strs);
    int val = 0;
    if (ret == -1) {
      ret = 1;
      val = exp->binop.left->binop.right->val;
    }
    return ret &&
           emitCond(exp->binop.left->binop.op, condLeft, condRight, labelFalse,
                    labelTrue, val) &&
           emitCompoundWhile(exp->binop.right, labelTrue, labelFalse, vars,
                             strs);
  }
  return 0;
}
int emitCompoundWhileOr(Exp exp, char *labelTrue, char *labelFalse, vars *vars,
                        stringLiterals **strs) {
  if (exp->tag == BINOP && exp->binop.op == OR) {
    if (exp->binop.left->tag == UNARYOP && exp->binop.left->unaryop.op == NOT)
      exp->binop.left = applyNotToExpr(exp->binop.left->unaryop.exp, 1, vars);

    if (exp->binop.left->tag == BINOP && exp->binop.left->binop.op == AND)
      return emitCompoundWhileAnd(exp->binop.left, labelTrue, labelFalse, vars,
                                  strs) &&
             emitCompoundWhile(exp->binop.right, labelTrue, labelFalse, vars,
                               strs);
    else if (exp->binop.left->tag == BINOP && exp->binop.left->binop.op == OR)
      return emitCompoundWhileOr(exp->binop.left, labelTrue, labelFalse, vars,
                                 strs) &&
             emitCompoundWhile(exp->binop.right, labelTrue, labelFalse, vars,
                               strs);

    int ret = transExp(exp->binop.left, NULL, vars, strs);
    int val = 0;
    if (ret == -1) {
      ret = 1;
      val = exp->binop.left->binop.right->val;
    }
    return ret &&
           emitCond(exp->binop.left->binop.op, condLeft, condRight, labelFalse,
                    labelTrue, val) &&
           emitCompoundWhile(exp->binop.right, labelTrue, labelFalse, vars,
                             strs);
  }
  return 0;
}

int emitCompoundIfAnd(Exp exp, char *labelTrue, char *labelFalse, vars *vars,
                      stringLiterals **strs) {
  if (exp->tag == BINOP && exp->binop.op == AND) {
    if (exp->binop.left->tag == UNARYOP && exp->binop.left->unaryop.op == NOT)
      exp->binop.left = applyNotToExpr(exp->binop.left->unaryop.exp, 1, vars);
    if (exp->binop.left->tag == BINOP && exp->binop.left->binop.op == AND)
      return emitCompoundIfAnd(exp->binop.left, labelTrue, labelFalse, vars,
                               strs) &&
             emitCompoundIf(exp->binop.right, labelTrue, labelFalse, vars,
                            strs);
    else if (exp->binop.left->tag == BINOP && exp->binop.left->binop.op == OR)
      return emitCompoundIfOr(exp->binop.left, labelTrue, labelFalse, vars,
                              strs) &&
             emitCompoundIf(exp->binop.right, labelTrue, labelFalse, vars,
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
  return 0;
}
int emitCompoundIfOr(Exp exp, char *labelTrue, char *labelFalse, vars *vars,
                     stringLiterals **strs) {
  if (exp->tag == BINOP && exp->binop.op == OR) {
    if (exp->binop.left->tag == UNARYOP && exp->binop.left->unaryop.op == NOT)
      exp->binop.left = applyNotToExpr(exp->binop.left->unaryop.exp, 1, vars);
    if (exp->binop.left->tag == BINOP && exp->binop.left->binop.op == AND)
      return emitCompoundIfAnd(exp->binop.left, labelTrue, labelFalse, vars,
                               strs) &&
             emitCompoundIf(exp->binop.right, labelTrue, labelFalse, vars,
                            strs);
    else if (exp->binop.left->tag == BINOP && exp->binop.left->binop.op == OR)
      return emitCompoundIfOr(exp->binop.left, labelTrue, labelFalse, vars,
                              strs) &&
             emitCompoundIf(exp->binop.right, labelTrue, labelFalse, vars,
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
           emitCond(exp->binop.left->binop.op, condLeft, condRight, labelTrue,
                    labelFalse, val) &&
           emitCompoundIf(exp->binop.right, labelTrue, labelFalse, vars, strs);
  }
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
    int neg = 0;

    if (stm->ifStmt.cond->tag == UNARYOP &&
        stm->ifStmt.cond->unaryop.op == NOT) {
      stm->ifStmt.cond = applyNotToExpr(stm->ifStmt.cond->unaryop.exp, 1, vars);
      if (!stm->ifStmt.cond) {
        fprintf(stderr, "Expression returned after applying NOT is NULL\n");
        return 0;
      }
    }

    if (stm->ifStmt.cond->binop.op == AND || stm->ifStmt.cond->binop.op == OR) {

      int ret =
          emitCompoundIf(stm->ifStmt.cond, labelTrue, labelFalse, vars, strs);
      if (!ret) {
        fprintf(stderr,
                "Unable to generate branches for the given condition\n");
        return 0;
      }
      ret = ret && emitLabel(labelTrue) &&
            transStm(stm->ifStmt.thenBranch, vars, strs) &&
            emitJump(labelEnd) && emitLabel(labelFalse);
      if (stm->ifStmt.elseBranch != NULL)
        ret = ret && transStm(stm->ifStmt.elseBranch, vars, strs);

      return ret && emitJump(labelEnd) && emitLabel(labelEnd);
      emitJump(labelEnd) && emitLabel(labelFalse);
      return ret;
    }
    if (stm->ifStmt.cond->tag == ID) {
      char *id = getVarTemp(stm->ifStmt.cond->id, vars);
      if (id == NULL) {
        fprintf(stderr, "Unable to find register for variable %s\n",
                stm->ifStmt.cond->id);
        return 0;
      }
      id = strdup(id);
      int ret = transExp(stm->ifStmt.cond, id, vars, strs) &&
                emitCond(NEQ, id, "zero", labelTrue, labelFalse, 0) &&
                emitLabel(labelTrue) &&
                transStm(stm->ifStmt.thenBranch, vars, strs) &&
                emitJump(labelEnd) && emitLabel(labelFalse);
      if (stm->ifStmt.elseBranch != NULL)
        ret = ret && transStm(stm->ifStmt.elseBranch, vars, strs);

      return ret && emitJump(labelEnd) &&

             emitLabel(labelEnd);
    }
    if (stm->ifStmt.cond->tag == BOOL || stm->ifStmt.cond->tag == NUM) {
      temp = newTemp();
      int ret = transExp(stm->ifStmt.cond, temp, vars, strs) &&
                emitCond(NEQ, temp, "zero", labelTrue, labelFalse, 0) &&
                emitLabel(labelTrue) &&
                transStm(stm->ifStmt.thenBranch, vars, strs) &&
                emitJump(labelEnd) && emitLabel(labelFalse);
      removeTemp(temp);

      if (stm->ifStmt.elseBranch != NULL)
        ret = ret && transStm(stm->ifStmt.elseBranch, vars, strs);

      return ret && emitJump(labelEnd) &&

             emitLabel(labelEnd);
    }

    int ret = transExp(stm->ifStmt.cond, temp, vars, strs) &&
              emitCond(stm->ifStmt.cond->binop.op, condLeft, condRight,
                       labelTrue, labelFalse, 0) &&

              emitLabel(labelTrue) &&
              transStm(stm->ifStmt.thenBranch, vars, strs) &&
              emitJump(labelEnd) &&

              emitLabel(labelFalse);
    if (stm->ifStmt.elseBranch != NULL)
      ret = ret && transStm(stm->ifStmt.elseBranch, vars, strs);

    return ret && emitJump(labelEnd) &&

           emitLabel(labelEnd);

    break;
  }
  case WHILE: {
    char *temp = NULL;
    char *labelFalse = newLabel();
    char *labelEnd = newLabel();
    int neg = 0;
    emitLabel(labelFalse);

    if (stm->whileStmt.cond->tag == UNARYOP &&
        stm->whileStmt.cond->unaryop.op == NOT)
      stm->whileStmt.cond =
          applyNotToExpr(stm->whileStmt.cond->unaryop.exp, 1, vars);

    if (stm->whileStmt.cond->binop.op == AND ||
        stm->whileStmt.cond->binop.op == OR) {

      int ret = emitCompoundWhile(stm->whileStmt.cond, labelEnd, labelFalse,
                                  vars, strs);
      if (!ret) {
        fprintf(stderr,
                "Unable to generate branches for the given condition\n");
        return 0;
      }
      return ret && transStm(stm->whileStmt.body, vars, strs) &&
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
                emitCond(NEQ, id, "zero", labelFalse, labelEnd, 0) &&
                transStm(stm->whileStmt.body, vars, strs) &&
                emitJump(labelFalse);

      return ret && emitJump(labelEnd) && emitLabel(labelEnd);
    }
    if (stm->whileStmt.cond->tag == BOOL || stm->whileStmt.cond->tag == NUM) {
      temp = newTemp();
      int ret = transExp(stm->whileStmt.cond, temp, vars, strs) &&
                emitCond(NEQ, temp, "zero", labelFalse, labelEnd, 0) &&
                transStm(stm->whileStmt.body, vars, strs) &&
                emitJump(labelFalse) && emitLabel(labelFalse);
      removeTemp(temp);

      return ret && emitJump(labelEnd) && emitLabel(labelEnd);
    }

    int ret = transExp(stm->whileStmt.cond, temp, vars, strs) &&
              emitCond(stm->whileStmt.cond->binop.op, condLeft, condRight,
                       labelFalse, labelEnd, 0) &&

              transStm(stm->whileStmt.body, vars, strs) && emitJump(labelFalse);

    return ret && emitJump(labelEnd) &&

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

int transExp(Exp exp, char *dest, vars *vars, stringLiterals **strs) {
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
    if (id == NULL)
      id = exp->id;
    else
      id = strdup(id);

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

void printInstructions(instrList *list) {
  printf("\nIR:\n");
  instrList *current = list;
  while (current != NULL) {
    switch (current->instr.opcode) {
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
