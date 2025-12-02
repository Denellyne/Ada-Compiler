#ifndef CODEGEN
#define CODEGEN
#include "../IR/ir.h"
#include "../Table/table.h"

int codeGen(Table tbl, Stm varDecl, InstrList *ir, stringLiterals *strs);

#endif
