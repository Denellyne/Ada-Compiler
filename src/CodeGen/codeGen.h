#ifndef CODEGEN
#define CODEGEN
#include "../IR/ir.h"
#include "../Table/table.h"

int generateASM(char *fileName, Table tbl, Stm varDecl, instrList *ir,
                stringLiterals *strs, floatLiterals *floats);

#endif
