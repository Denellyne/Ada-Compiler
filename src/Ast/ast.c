#include "ast.h"
#include <stdio.h>
Exp mkNum(double v) {
  Exp e = (Exp)malloc(sizeof(struct _exp));
  e->tag = NUM;
  e->val = v;
  return e;
}
Exp mkBinOp(Exp lExp, binop op, Exp rExp) {

  Exp e = (Exp)malloc(sizeof(struct _exp));
  e->tag = OP;
  e->binop.left = lExp;
  e->binop.right = rExp;
  e->binop.op = op;
  return e;
}

Exp mkId(char *id) {

  Exp e = (Exp)malloc(sizeof(struct _exp));
  e->tag = ID;
  e->id = id;
  return e;
}
Stm mkCompound(Stm lStm, Stm rStm) {

  Stm ptr = (Stm)malloc(sizeof(struct _stm));
  ptr->tag = COMPOUND;
  ptr->compound.fst = lStm;
  ptr->compound.snd = rStm;
  return ptr;
}

Stm mkAssign(char *id, Exp exp) {

  Stm ptr = (Stm)malloc(sizeof(struct _stm));
  ptr->tag = ASSIGN;
  ptr->assign.ident = id;
  ptr->assign.expr = exp;
  return ptr;
}
Stm mkIncr(char *id) {

  Stm ptr = (Stm)malloc(sizeof(struct _stm));
  ptr->tag = INCR;
  ptr->ident = id;
  return ptr;
}
Func mkFunc(char *id, int returnValue, int numArgs, ...) {

  Func ptr = (Func)malloc(sizeof(struct _func));
  ptr->ident = id;
  ptr->returnValueTag = returnValue;
  ptr->numArgs = numArgs;

  Arg args = ptr->args;

  va_list list;
  va_start(list, numArgs);
  for (int i = 0; i < numArgs; i++) {
    *args = *va_arg(list, Arg);
    args = (*args).nextArg;
  }
  va_end(list);
  return ptr;
}

void printStm(Stm ptr) {

  if (ptr == NULL)
    return;
  switch (ptr->tag) {
  case ASSIGN:
    printf("%s", ptr->assign.ident);
    printf("=");
    printExp(ptr->assign.expr);
    printf("; ");
    break;
  case INCR:
    printf("%s++", ptr->ident);
    printf("; ");
    break;
  case COMPOUND:
    printStm(ptr->compound.fst);
    printStm(ptr->compound.snd);
    break;
  }
}

void printExp(Exp ptr) {

  switch (ptr->tag) {
  case NUM:
    printf("%f", ptr->val);
    break;
  case ID:
    printf("%s", ptr->id);
    break;
  case OP:
    printf("(");
    printExp(ptr->binop.left);
    printOp(ptr->binop.op);
    printExp(ptr->binop.right);
    printf(")");
    break;
  }
}

void printOp(binop op) {
  switch (op) {
  case PLUS:
    printf("+");
    break;
  case MINUS:
    printf("-");
    break;
  case TIMES:
    printf("*");
    break;
  case DIV:
    printf("/");
    break;
  }
}
