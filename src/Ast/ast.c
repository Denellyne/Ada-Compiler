#include "ast.h"
#include <stdio.h>
#include <stdlib.h>
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
Exp mkBinOp(Exp lExp, op op, Exp rExp) {

  Exp e = (Exp)malloc(sizeof(struct _exp));
  e->tag = BINOP;
  e->binop.left = lExp;
  e->binop.right = rExp;
  e->binop.op = op;
  return e;
}

Exp mkUnaryOp(Exp exp, op op) {
  Exp e = (Exp)malloc(sizeof(struct _exp));
  e->tag = UNARYOP;
  e->unaryop.op = op;
  e->unaryop.exp = exp;
  return e;
}

Exp mkId(char *id) {

  Exp e = (Exp)malloc(sizeof(struct _exp));
  e->tag = ID;
  e->id = strdup(id);
  return e;
}

Exp mkBool(int b) {
  Exp e = (Exp)malloc(sizeof(struct _exp));
  e->tag = BOOL;
  e->bool_val = b;
  return e;
}

Prog mkProg(Stm varDec, Stm statments) {

  Prog ptr = (Prog)malloc(sizeof(struct _prog));
  ptr->varDec = varDec;
  ptr->statements = statments;
  ptr->ident = "Prog";
  return ptr;
}
Stm mkCompound(Stm lStm, Stm rStm) {

  Stm ptr = (Stm)malloc(sizeof(struct _stm));
  ptr->tag = COMPOUND;
  ptr->compound.fst = lStm;
  ptr->compound.snd = rStm;
  return ptr;
}

Stm mkAssign(char *id, int type, Exp exp) {

  Stm ptr = (Stm)malloc(sizeof(struct _stm));
  ptr->tag = ASSIGN;
  ptr->assign.ident = id;
  if (!exp) {
    switch (type) {
    case NUM:
      exp = mkNum(0);
      break;
    case STRLITERAL: {
      char *str = (char *)malloc(sizeof(char) * 3);
      str[0] = ' ';
      str[1] = '\0';
      exp = mkStringLiteral(str);
      free(str);
    } break;
    case BOOL:
      exp = mkBool(0);
      break;
    default:
      exp = NULL;
      break;
    }
  }
  if (!exp)
    return NULL;
  ptr->assign.expr = exp;

  if (type == -1)
    ptr->assign.type = exp->tag;
  else
    ptr->assign.type = type;
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

Stm mkIf(Exp cond, Stm thenBranch, Stm elseifBranch, Stm elseBranch) {
  Stm ptr = (Stm)malloc(sizeof(struct _stm));
  ptr->tag = IF;
  ptr->ifStmt.cond = cond;
  ptr->ifStmt.thenBranch = thenBranch;
  ptr->ifStmt.elsifBranch = elseifBranch;
  ptr->ifStmt.elseBranch = elseBranch;
  return ptr;
}
Stm mkWhile(Exp cond, Stm body) {
  Stm ptr = (Stm)malloc(sizeof(struct _stm));
  ptr->tag = WHILE;
  ptr->whileStmt.cond = cond;
  ptr->whileStmt.body = body;
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
  return root;
}

Func mkFunc(char *id, int returnValue, Stm args) {

  Func ptr = (Func)malloc(sizeof(struct _func));
  ptr->ident = id;
  ptr->returnValueTag = returnValue;
  ptr->args = args;

  return ptr;
}
void printProg(Prog ptr) {
  if (!ptr)
    return;
  printf("%s ", ptr->ident);
  printf("VarDecl ");
  printf("Statements ");
  if (!ptr->varDec)
    printf("None ");
  else
    printStm(ptr->varDec);
  printf("(");
  printStm(ptr->statements);
  printf(")");
}

void printStm(Stm ptr) {

  if (!ptr)
    return;
  switch (ptr->tag) {
  case ASSIGN:
    printf("(");
    printf("%s", ptr->assign.ident);
    switch (ptr->assign.type) {
    case ID:
      printf(" ID");
      break;
    case NUM:
      printf(" INTEGER");
      break;
    case STRLITERAL:
      printf(" STRING");
      break;
    case BOOL:
      printf(" BOOL");
      break;
    default:
      break;
    }
    if (ptr->assign.expr) {
      printf(" (");
      printExp(ptr->assign.expr);
      printf(")");
    }
    printf(")");

    break;
  case INCR:
    printf("%s++", ptr->ident);
    break;
  case COMPOUND:
    printStm(ptr->compound.fst);
    printf(" ");
    printStm(ptr->compound.snd);
    break;
  case FUNCTION: {
    printf("%s", ptr->function.ident);
    printf("(");
    printArgs(ptr->function.args);
    printf(")");
  } break;
  case IF: {

    int hasElse = 0;
    int hasElsif = 0;
    printf("IF");
    printf(" THEN ");
    if (ptr->ifStmt.elsifBranch) {
      printf(" ELSIF ");
      hasElsif = 1;
    }

    if (ptr->ifStmt.elseBranch) {
      printf(" ELSE ");
      hasElse = 1;
    }

    printExp(ptr->ifStmt.cond);
    printf(" (");
    printStm(ptr->ifStmt.thenBranch);
    printf(")");

    if (hasElsif) {
      printf(" (");
      printStm(ptr->ifStmt.elsifBranch);
      printf(")");
    }
    if (hasElse) {
      printf(" (");
      printStm(ptr->ifStmt.elseBranch);
      printf(")");
    }
  } break;
  case WHILE:
    printf("WHILE");
    printf(" DO ");
    printExp(ptr->whileStmt.cond);
    printf(" ");
    printStm(ptr->whileStmt.body);
    break;
  }
}

void printArgs(Arg arg) {
  if (!arg)
    return;
  while (arg) {
    printExp(arg->arg);
    arg = arg->nextArg;
    if (arg)
      printf(" ");
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
  case BINOP:
    printf("(");
    printOp(ptr->binop.op);
    printExp(ptr->binop.left);
    printf(" ");
    printExp(ptr->binop.right);
    printf(")");
    break;
  case UNARYOP:
    printf("(");
    printOp(ptr->unaryop.op);
    printExp(ptr->unaryop.exp);
    printf(")");
    break;
  case STRLITERAL:
    printf("%s", ptr->str);
    break;
  case BOOL:
    if (ptr->bool_val)
      printf("TRUE");
    else
      printf("FALSE");
    break;
  }
}

void printOp(op op) {
  switch (op) {
  case POW:
    printf("POW ");
    break;
  case PLUS:
    printf("PLUS ");
    break;
  case MINUS:
    printf("MINUS ");
    break;
  case TIMES:
    printf("MULT ");
    break;
  case DIV:
    printf("DIV ");
    break;
  case AND:
    printf("AND ");
    break;
  case OR:
    printf("OR ");
    break;
  case NOT:
    printf("NOT ");
    break;
  case EQ:
    printf("EQUAL ");
    break;
  case NEQ:
    printf("NOT EQUAL ");
    break;
  case LT:
    printf("LESSER ");
    break;
  case GT:
    printf("GREATER ");
    break;
  case LE:
    printf("LESS EQUAL ");
    break;
  case GE:
    printf("GREAT EQUAL ");
    break;
  case XOR:
    printf("XOR ");
    break;
  }
}
