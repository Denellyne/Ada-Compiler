#ifndef AST
#define AST
#define INT 0
#define STRING 1
#define VOID 2

#include <stdarg.h>
#include <stdlib.h>
typedef enum { PLUS, MINUS, TIMES, DIV } binop;
struct _exp {
  enum { ID, NUM, OP } tag;
  union {
    double val; // for NUM
    char *id;   // for ID
    struct {    // for OP
      binop op;
      struct _exp *left, *right;
    } binop;
  };
};
typedef struct _exp *Exp;
struct _stm {
  enum { COMPOUND, ASSIGN, INCR } tag;
  union {
    struct { // for COMPOUND
      struct _stm *fst, *snd;
    } compound;
    struct { // for ASSIGN
      char *ident;
      struct _exp *expr;
    } assign;
    char *ident; // for INCR
  };
};
typedef struct _stm *Stm;

struct _args {
  char *id;
  int typeTag;
  struct _args *nextArg;
};
typedef struct _args *Arg;

struct _func {
  enum { rVoid, rInt, rString } returnValue;

  char *ident;
  int returnValueTag;
  int numArgs;
  struct _args *args;
  Stm stm;
};
typedef struct _func *Func;

Exp mkNum(double v);
Exp mkBinOp(Exp lExp, binop op, Exp rExp);
Exp mkId(char *id);
Stm mkCompound(Stm lStm, Stm rStm);
Stm mkAssign(char *id, Exp exp);
Stm mkIncr(char *id);
Func mkFunc(char *id, int returnValue, int numArgs, ...);
void printStm(Stm);
void printExp(Exp);
void printOp(binop op);
#endif // !TOKENS
