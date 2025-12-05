#ifndef IR
#define IR

#include "../Ast/ast.h"

typedef enum {
  MOVE,
  MOVEI,
  OP,
  LABEL,
  JUMP,
  COND,
  CALL,
  LOADADRESS,
  ADD,
  ADDI,
  SUB,
  SUBI,
  MULT,
  DIVIDE,
  MULTI,
  DIVIDEI,
  POWER,
  POWERI,
  UNARY,
  NEG,
} Opcode;
struct _variables {
  char *id;
  char *temp;
  struct _variables *next;
};
typedef struct _variables vars;
struct _stringLiterals {
  char *id;
  char *str;
  struct _stringLiterals *next;
};
typedef struct _stringLiterals stringLiterals;

struct _instruction {
  Opcode opcode;
  char *arg1;
  char *arg2;
  char *arg3;
  char *arg4;
  int num;
  op binop;
};
typedef struct _instruction instruction;

struct _instrList {
  instruction instr;
  struct _instrList *next;
};
typedef struct _instrList instrList;

instrList *generateIR(Prog program, stringLiterals **strs);
void printInstructions(instrList *list);
void freeInstructions(instrList **list);
void freeStrings(stringLiterals **strs);

#endif
