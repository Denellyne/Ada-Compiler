#ifndef AST
#define AST

#include <stdlib.h>
typedef enum {
  POW,
  PLUS,
  MINUS,
  TIMES,
  DIV, // operadores aritméticos
  AND,
  OR,
  NOT,
  XOR,
  EQ,
  NEQ,
  LT,
  GT,
  LE,
  GE // operadores de comparação
} op;
struct _exp {
  enum { ID, NUM, BINOP, BOOL, STRLITERAL, UNARYOP } tag;
  union {
    double val; // for NUM
    char *id;   // for ID
    char *str;  // for STRLITERAL
    int bool_val;
    struct { // for OP
      op op;
      struct _exp *left, *right;
    } binop;
    struct {
      op op;
      struct _exp *exp;
    } unaryop;
  };
};
typedef struct _exp *Exp;

struct _args {
  Exp arg;
  struct _args *nextArg;
};
typedef struct _args *Arg;

struct _stm {
  enum {
    COMPOUND,
    ASSIGN,
    INCR,
    FUNCTION,
    IF,
    WHILE,
  } tag;
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
    struct { // for IF
      Exp cond;
      struct _stm *thenBranch;
      struct _stm *elsifBranch;
      struct _stm *elseBranch;
    } ifStmt;
    struct { // for WHILE
      Exp cond;
      struct _stm *body;
    } whileStmt;
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
struct _prog {
  char *ident;
  Stm varDec;
  Stm statements;
};
typedef struct _prog *Prog;

Exp mkStringLiteral(char *stringLiteral);
Exp mkId(char *id);
Exp mkBool(int b);
Exp mkNum(double v);
Exp mkBinOp(Exp lExp, op op, Exp rExp);
Exp mkUnaryOp(Exp exp, op op);
Stm mkCompound(Stm lStm, Stm rStm);
Stm mkAssign(char *id, int type, Exp exp);
Stm mkIncr(char *id);
Stm mkArgList(Arg);
Stm mkFuncCall(char *id, Arg args);
Stm mkIf(Exp cond, Stm thenBranch, Stm elseifBranch, Stm elseBranch);
Stm mkWhile(Exp cond, Stm body);

Prog mkProg(Stm varDec, Stm statments);
Arg mkArg(Exp expr);
Arg appendArg(Arg root, Arg newArg);

Func mkFunc(char *id, int returnValue, Stm args);

void printProg(Prog);
void printStm(Stm);
void printExp(Exp);
void printOp(op op);
void printFunc(Func);
void printArgs(Arg);
#endif // !TOKENS
