
/* Simple key-value table implemented as a linked list
 */

#include "table.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int convertType(int type) {

  switch (type) {
  case FLOAT:
    return TBL_FLOAT;
  case NUM:
    return TBL_INT;
  case STRLITERAL:
    return TBL_STRING;
  case BOOL:
    return TBL_BOOL;
  case ID:
    return TBL_ID;
  default:
    return TBL_ERROR;
  }
}

int checkType(Table tbl, char *name) {

  while (tbl != NULL) {
    if (strcmp(tbl->key, name) == 0)
      return tbl->typeTag;
    tbl = tbl->next;
  }
  return TBL_ERROR;
}
int checkExprType(Table tbl, Exp expr) {
  switch (expr->tag) {
  case FLOAT:
    return TBL_FLOAT;
  case BOOL:
    return TBL_BOOL;
  case NUM:
    return TBL_INT;
  case STRLITERAL:
    return TBL_STRING;
  case ID:
    return checkType(tbl, expr->id);
  case UNARYOP: {

    if (expr->unaryop.op == NOT) {
      int ret = checkExprType(tbl, expr->unaryop.exp);
      if (ret != TBL_BOOL && ret != TBL_INT)
        return TBL_ERROR;
      return ret;
    }
    if (expr->unaryop.op == PLUS || expr->unaryop.op == MINUS) {
      int ret = checkExprType(tbl, expr->unaryop.exp);
      if (ret == TBL_INT)
        return TBL_INT;
      else if (ret == TBL_FLOAT)
        return TBL_FLOAT;
      return TBL_ERROR;
    }
  } break;
  case BINOP: {

    switch (expr->binop.op) {
    case POW: {
      int typeL = checkExprType(tbl, expr->binop.left);
      int typeR = checkExprType(tbl, expr->binop.right);
      if (typeL & (TBL_INT | TBL_FLOAT) && (typeR & TBL_INT)) {
        if (typeL == TBL_FLOAT)
          return TBL_FLOAT;
        return TBL_INT;
      }
      return TBL_ERROR;
    } break;
    case PLUS:
    case MINUS:
    case TIMES:
    case DIV: {

      int typeL = checkExprType(tbl, expr->binop.left);
      int typeR = checkExprType(tbl, expr->binop.right);
      if (typeL & (TBL_INT | TBL_FLOAT) && (typeR & (TBL_FLOAT | TBL_INT))) {
        if (typeL == TBL_FLOAT && typeR == TBL_FLOAT)
          return TBL_FLOAT;
        return TBL_INT;
      }
      return TBL_ERROR;
    } break;
    case XOR: {

      int left = checkExprType(tbl, expr->binop.left);
      int right = checkExprType(tbl, expr->binop.right);
      if ((left & right) == 0)
        return TBL_ERROR;
      if (left & TBL_INT)
        return TBL_INT;
      else if (left & TBL_BOOL)
        return TBL_BOOL;
      return TBL_ERROR;
    } break;
    case EQ:
    case NEQ:
    case LT:
    case GT:
    case LE:
    case GE: {

      int left = checkExprType(tbl, expr->binop.left);
      int right = checkExprType(tbl, expr->binop.right);
      if (left == TBL_ERROR || right == TBL_ERROR) {
        fprintf(stderr, "One of the operands on a comparison was an error\n");
        return TBL_ERROR;
      }
      if (left & (TBL_INT | TBL_BOOL) && (right & (TBL_BOOL | TBL_INT)))
        return left & right;

      fprintf(stderr,
              "Invalid type for operation, Wanted:TBL_INT Got:Left=%d "
              "Right=%d Result:%d\n",
              left, right, left && right && TBL_INT);

      return TBL_ERROR;
    } break;
    case AND:
    case OR: {

      int left = checkExprType(tbl, expr->binop.left);
      int right = checkExprType(tbl, expr->binop.right);
      if (left & (TBL_INT | TBL_BOOL) && right & ((TBL_BOOL | TBL_INT)))
        return left && right;
      fprintf(stderr,
              "Invalid type for operation, Wanted:TBL_BOOL Got:Left=%d "
              "Right=%d Result:%d\n",
              left, right, left && right && TBL_BOOL);

      return TBL_ERROR;
    } break;
    }
  } break;
  }
  if (expr->tag == BINOP) {
    int left = checkExprType(tbl, expr->binop.right);
    int right = checkExprType(tbl, expr->binop.left);
    if (left == right)
      return left;
    return TBL_ERROR;
  }
  if (expr->tag == ID)
    return checkType(tbl, expr->id);
  return convertType(expr->tag);
}

/* Lookup a name in a table; returns entry or NULL if it doesn't occur
 */
Entry *lookup(Table tbl, char *name) {
  while (tbl != NULL) {
    if (strcmp(tbl->key, name) == 0)
      return tbl;
    tbl = tbl->next;
  }
  return NULL;
}

/* Add an entry to the begining of a table
 */
Table addEntry(Table tbl, char *name, int type, unsigned numArgs, ...) {
  Entry *ptr = malloc(sizeof(Entry));
  ptr->key = name;
  ptr->typeTag = type;
  ptr->next = tbl;
  ptr->numArgs = numArgs;
  if (numArgs > 0) {
    va_list argptr;
    int type = TBL_ERROR;
    struct _argType *head = NULL;
    struct _argType *tail = NULL;

    va_start(argptr, numArgs);
    while (numArgs--) {
      type = va_arg(argptr, int);
      if (!head) {
        tail = (struct _argType *)malloc(sizeof(struct _argType));
        tail->typeTag = type;
        tail->next = NULL;
        if (!tail) {
          fprintf(stderr, "Unable to malloc ptr for argType struct\n");
          return NULL;
        }
        head = tail;
      } else {
        tail->next = (struct _argType *)malloc(sizeof(struct _argType));
        tail->next->typeTag = type;
        tail->next->next = NULL;
        if (!tail) {
          fprintf(stderr, "Unable to malloc ptr for argType struct\n");
          return NULL;
        }
        type = TBL_ERROR;
        tail = tail->next;
      }
    }
    ptr->argType = head;
    va_end(argptr);
  } else
    ptr->argType = NULL;
  return ptr;
}
int validateExprType(Table tbl, int typeTag, int exprType) {

  if (exprType == TBL_ERROR)
    return 2;

  if (typeTag != exprType)
    return 3;

  return 1;
}
int validateExprTypeEx(Table tbl, Exp expr, int typeTag) {
  int exprType = checkExprType(tbl, expr);

  return validateExprType(tbl, typeTag, exprType);
}
Table addVariableDeclarations(Table tbl, Stm varDecl) {
  if (!varDecl)
    return tbl;
  if (lookup(tbl, varDecl->compound.fst->ident) != NULL) {
    fprintf(stderr, "Variable %s declared twice\n",
            varDecl->compound.fst->ident);
    return NULL;
  }

  int typeTag = convertType(varDecl->compound.fst->assign.type);
  int exprType = checkExprType(tbl, varDecl->compound.fst->assign.expr);
  int ret = 0;

  if (ret = validateExprType(tbl, typeTag, exprType), ret == 3) {

    fprintf(stderr, "Variable %s is assigned to expression of different type\n",
            varDecl->compound.fst->assign.ident);
    return NULL;

  } else if (ret == 2) {
    fprintf(stderr, "Variable %s is assigned to malformed expression\n",
            varDecl->compound.fst->assign.ident);
    return NULL;
  }

  tbl = addEntry(tbl, varDecl->compound.fst->assign.ident, typeTag, 0);
  return addVariableDeclarations(tbl, varDecl->compound.snd);
}
int validateAST(Table tbl, Stm stm) {
  if (!tbl)
    return 0;
  if (!stm)
    return 1;
  switch (stm->tag) {

  case COMPOUND:
    return validateAST(tbl, stm->compound.fst) &&
           validateAST(tbl, stm->compound.snd);
    break;
  case ASSIGN: {

    Entry *entry = lookup(tbl, stm->assign.ident);
    if (!entry) {
      fprintf(stderr, "Unable to find varialbe in the symbol table\n");
      return 0;
    }
    int typeTag = entry->typeTag;
    int exprType = checkExprType(tbl, stm->assign.expr);
    int ret = 0;

    if (ret = validateExprType(tbl, typeTag, exprType), ret == 3) {
      fprintf(stderr,
              "Variable %s is assigned to expression of different type\n",
              stm->assign.ident);
      return 0;
    } else if (ret == 2) {
      fprintf(stderr, "Variable %s is assigned to malformed expression\n",
              stm->assign.ident);
      return 0;
    }
  } break;
  case FUNCTION: {
    printf("%s\n", stm->function.ident);
    Table entry = lookup(tbl, stm->function.ident);
    if (!entry) {
      fprintf(stderr, "Unable to find symbol for the given function\n");
      return 0;
    }
    Arg head = stm->function.args;
    struct _argType *headArg = entry->argType;
    int counter = 0;
    while (head) {
      if (!headArg) {
        fprintf(stderr,
                "Number of arguments given doesn't correspond to "
                "function signature\n Number of arguments passed:%d\n "
                "Number of "
                "arguments in function signature:%d\n",
                counter, entry->numArgs);
        return 0;
      }
      counter++;
      if ((headArg->typeTag & TBL_ID)) {
        if (head->arg->tag != ID || !lookup(tbl, head->arg->id)) {
          fprintf(stderr,
                  "Passed argument needs to be a variable in function %s\n",
                  stm->function.ident);
          return 0;
        }
      }
      int exprType = checkExprType(tbl, head->arg);
      if (((headArg->typeTag & (TBL_ID - 1)) && exprType) == 0) {
        fprintf(stderr, "Passed argument of wrong type to function %s\n",
                stm->function.ident);
        return 0;
      }
      headArg = headArg->next;
      head = head->nextArg;
    }
    if (headArg && !head) {
      fprintf(stderr,
              "Number of arguments given doesn't correspond to "
              "function signature\n Number of arguments passed:%d\n "
              "Number of "
              "arguments in function signature:%d\n",
              counter, entry->numArgs);
      return 0;
    }

    if (counter != entry->numArgs) {
      fprintf(stderr,
              "Number of arguments given doesn't correspond to "
              "function signature\n Number of arguments passed:%d\n "
              "Number of "
              "arguments in function signature:%d\n",
              counter, entry->numArgs);
      return 0;
    }

  } break;

  case IF: {

    int type = checkExprType(tbl, stm->ifStmt.cond);
    if (type != TBL_BOOL && type != TBL_INT) {
      fprintf(stderr, "Condition isn't of type Bool, type received:%d\n", type);
      return 0;
    }
    return validateAST(tbl, stm->ifStmt.thenBranch) &&
           // validateAST(tbl, stm->ifStmt.elsifBranch) &&
           validateAST(tbl, stm->ifStmt.elseBranch);
  } break;

  case WHILE:
    return validateAST(tbl, stm->whileStmt.body);
    break;
  }
  return 1;
}
void printTableRecur(Table tbl) {
  if (!tbl)
    return;
  printf("%s %d\n", tbl->key, tbl->typeTag);
  return printTableRecur(tbl->next);
}
void printTable(Table tbl) {
  printf("\nTable:\n");
  if (!tbl)
    return;
  printf("%s %d\n", tbl->key, tbl->typeTag);
  return printTableRecur(tbl->next);
}
