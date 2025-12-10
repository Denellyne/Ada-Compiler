#ifndef OPTIMIZER
#define OPTIMIZER
#include "../IR/ir.h"

instrList *optimizeIR(instrList *ir, stringLiterals **strs,
                      floatLiterals **floats, int *changed);

#endif // !OPTIMIZER
