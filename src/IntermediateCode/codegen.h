#ifndef CODEGEN_H
#define CODEGEN_H 

#include "../Ast/ast.h"
#include "../Table/table.h"

typedef enum {
    MOVE, MOVEI, OP, OPI, LABEL, JUMP, COND
} Opcode;

typedef struct {
    Opcode opcode;
    char* arg1; 
    char* arg2;
    char* arg3;
    int num;
    binop binop;
} Instruction;

typedef struct InstrList {
Instruction instr;
struct InstrList* next;
} InstrList;

InstrList* genCode(Stm program);
void printInstructions(InstrList* list);
void freeInstructions(InstrList* list);

void emit2(Opcode opc, char* arg1, char* arg2);
void emit3(Opcode opc, char* arg1, char* arg2, char* arg3);
void emitMovel(char* dest, int num);
void emitOp(binop op, char* dest, char* src1, char* src2);
void emitCond(binop op, char* src1, char* src2, char* label1, char* label2); 
void emitLabel(char* label);
void emitJump(char* label);

char* newTemp();
char* newLabel();

void transStm(Stm stm);
void transExp(Exp exp, char* dest);
void transBinOp(Exp exp, char* dest);

#endif