#ifndef CODEGEN
#define CODEGEN
#include "../IR/ir.h"
#include "../Table/table.h"

int generateASM(Table tbl, Stm varDecl, instrList *ir, stringLiterals *strs);

#endif
