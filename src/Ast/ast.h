#ifndef AST
#define AST

#include <stdarg.h>
#include <stdlib.h>
typedef enum { PLUS, MINUS, TIMES, DIV } binop;
struct _exp {
  enum { ID, NUM, OP, STRLITERAL } tag;
  union {
    double val; // for NUM
    char *id;   // for ID
    char *str;  // for STRLITERAL
    struct {    // for OP
      binop op;
      struct _exp *left, *right;
    } binop;
  };
};
typedef struct _exp *Exp;

struct _args {
  Exp arg;
  struct _args *nextArg;
};
typedef struct _args *Arg;

struct _stm {
  enum { COMPOUND, ASSIGN, INCR, FUNCTION } tag;
  union {
    struct { // for COMPOUND
      struct _stm *fst, *snd;
    } compound;
    struct { // for ASSIGN
      char *ident;
      int type;
      struct _exp *expr;
    } assign;
    char *ident; // for INCR
    struct {
      struct _args *args;
      char *ident;
    } function;
  };
};
typedef struct _stm *Stm;

struct _func {
  enum { rVoid, rInt, rString } returnValue;

  char *ident;
  int returnValueTag;
  int numArgs;
  Stm args;
  Stm stm;
};
typedef struct _func *Func;

Exp mkStringLiteral(char *stringLiteral);
Exp mkId(char *id);
Exp mkNum(double v);
Exp mkBinOp(Exp lExp, binop op, Exp rExp);
Stm mkCompound(Stm lStm, Stm rStm);
Stm mkAssign(char *id, int type, Exp exp);
Stm mkIncr(char *id);
Stm mkArgList(Arg);
Stm mkFuncCall(char *id, Arg args);
Arg mkArg(Exp expr);
Arg appendArg(Arg root, Arg newArg);

Func mkFunc(char *id, int returnValue, Stm args);

void printStm(Stm);
void printExp(Exp);
void printOp(binop op);
void printFunc(Func);
void printArgs(Arg);
#endif // !TOKENS
