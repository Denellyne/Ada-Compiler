#ifndef AST
#define AST

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

Exp mkNum(double v);
Exp mkBinOp(Exp lExp, binop op, Exp rExp);
Exp mkId(char *id);
Stm mkCompound(Stm lStm, Stm rStm);
Stm mkAssign(char *id, Exp exp);
Stm mkIncr(char *id);
void printStm(Stm);
void printExp(Exp);
void printOp(binop op);
#endif // !TOKENS
