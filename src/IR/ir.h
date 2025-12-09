#ifndef IR
#define IR

#include "../Ast/ast.h"
static const char *temps[18] = {"t0", "t1", "t2", "t3", "t4", "t5",
                                "t6", "t7", "t8", "t9", "s0", "s1",
                                "s2", "s3", "s4", "s5", "s6", "s7"};

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
  GREATEREQ,
  LESSEREQ,
  EQUALS,
  NOTEQUALS,
  GREATER,
  LESSER,
  GREATEREQI,
  LESSEREQI,
  EQUALSI,
  NOTEQUALSI,
  GREATERI,
  LESSERI,
  XR,
  XRI,
  SAVEREGISTERS,
  LOADREGISTERS,
  BNEZ = 64,
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
