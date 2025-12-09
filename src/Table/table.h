#ifndef TABLE
#define TABLE
#include "../Ast/ast.h"
/* Simple linked list implementation for a key-value table
 */
struct _entry {
  char *key;
  enum {
    TBL_INT = 1,
    TBL_BOOL = 2,
    TBL_STRING = 4,
    TBL_FUNCTION = 8,
    TBL_ID = 16,
    TBL_FLOAT = 32,
    TBL_ERROR = 64,
  } typeTag;
  unsigned numArgs;
  struct _argType {
    int typeTag;
    struct _argType *next;
  } *argType;
  struct _entry *next;
};

typedef struct _entry Entry;

typedef Entry *Table;

// extern struct type lookup_value(Table, char *);
Entry *lookup(Table, char *);
Table addEntry(Table, char *, int type, unsigned numArgs, ...);
// extern void update_value(Entry *, struct type);
Table addVariableDeclarations(Table, Stm);
void printTable(Table);
int validateAST(Table, Stm);
#endif
