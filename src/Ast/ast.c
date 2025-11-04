#include "ast.h"
#include <stdio.h>
#include <string.h>

Exp mkStringLiteral(char *stringLiteral) {
  Exp e = (Exp)malloc(sizeof(struct _exp));
  e->tag = STRLITERAL;
  e->str = strdup(stringLiteral);
  return e;
}

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
  e->id = strdup(id);
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

Stm mkFuncCall(char *id, Arg args) {

  Stm ptr = (Stm)malloc(sizeof(struct _stm));
  ptr->tag = FUNCTION;
  ptr->function.args = args;

  ptr->function.ident = strdup(id);
  return ptr;
}
Arg mkArg(Exp expr) {
  Arg ptr = (Arg)malloc(sizeof(struct _args));
  ptr->arg = expr;
  ptr->nextArg = NULL;
  return ptr;
}
Arg appendArg(Arg root, Arg newArg) {
  Arg head = root;
  while (head->nextArg)
    head = head->nextArg;

  head->nextArg = newArg;

  if (!root)
    printf("???\n");
  else
    printExp(root->arg);
  return root;
}

Func mkFunc(char *id, int returnValue, Stm args) {

  Func ptr = (Func)malloc(sizeof(struct _func));
  ptr->ident = id;
  ptr->returnValueTag = returnValue;
  ptr->args = args;

  return ptr;
}

void printStm(Stm ptr) {

  if (!ptr)
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
  case FUNCTION: {
    printf("%s=", ptr->function.ident);

    printArgs(ptr->function.args);
  } break;
  }
}

void printArgs(Arg arg) {
  if (!arg)
    return;
  while (arg) {
    printExp(arg->arg);
    arg = arg->nextArg;
  }
}
void printExp(Exp ptr) {
  if (!ptr)
    return;

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
  case STRLITERAL:
    printf("%s", ptr->str);
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
