#ifndef IR
#define IR

#include "../Ast/ast.h"

typedef enum { MOVE, MOVEI, OP, LABEL, JUMP, COND, CALL, LOADADRESS } Opcode;
struct vars {
  char *id;
  char *temp;
  struct vars *next;
};
struct _stringLiterals {
  char *id;
  char *str;
  struct _stringLiterals *next;
};
typedef struct _stringLiterals stringLiterals;

typedef struct {
  Opcode opcode;
  char *arg1;
  char *arg2;
  char *arg3;
  char *arg4;
  int num;
  op binop;
} Instruction;

typedef struct InstrList {
  Instruction instr;
  struct InstrList *next;
} InstrList;

InstrList *genCode(Prog program, stringLiterals **strs);
void printInstructions(InstrList *list);
void freeInstructions(InstrList *list);

int emit2(Opcode opc, char *arg1, char *arg2);
int emit3(Opcode opc, char *arg1, char *arg2, char *arg3);
int emitMovel(char *dest, int num);
int emitOp(op op, char *dest, char *src1, char *src2);
int emitCond(op op, char *src1, char *src2, char *label1, char *label2);
int emitLabel(char *label);
int emitJump(char *label);
int emitFunction(char *id, char *temp, char *temp2);

char *newTemp();
char *newLabel();

int transStm(Stm stm, struct vars *, stringLiterals **strs);
int transExp(Exp exp, char *dest, struct vars *, stringLiterals **strs);
int transBinOp(Exp exp, char *dest, struct vars *, stringLiterals **strs);

#endif
