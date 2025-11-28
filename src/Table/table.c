
/* Simple key-value table implemented as a linked list
 */

#include "table.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int convertType(int type) {

  switch (type) {
  case NUM:
    return TBL_INT;
  case STRLITERAL:
    return TBL_STRING;
  case BOOL:
    return TBL_BOOL;
  case ID:
    return TBL_ID;
  default:
    return type;
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
  if (expr->tag == UNARYOP)
    return checkExprType(tbl, expr->unaryop.exp);
  if (expr->tag == BINOP) {
    int right = checkExprType(tbl, expr->binop.left);
    int left = checkExprType(tbl, expr->binop.right);
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
Table addEntry(Table tbl, char *name, int type) {
  Entry *ptr = malloc(sizeof(Entry));
  ptr->key = name;
  ptr->typeTag = type;
  ptr->next = tbl;
  return ptr;
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
  if (exprType == TBL_ERROR) {

    fprintf(stderr, "Variable %s is assigned to malformed expression\n",
            varDecl->compound.fst->ident);
    return NULL;
  }
  if (typeTag != exprType) {
    fprintf(stderr, "Variable %s is assigned to expression of different type\n",
            varDecl->compound.fst->ident);
    return NULL;
  }

  tbl = addEntry(tbl, varDecl->compound.fst->assign.ident, typeTag);
  return addVariableDeclarations(tbl, varDecl->compound.snd);
}
void printTable(Table tbl) {
  printf("\nTable:\n");
  if (!tbl)
    return;
  printf("%s %d\n", tbl->key, tbl->typeTag);
  return printTable(tbl->next);
}
