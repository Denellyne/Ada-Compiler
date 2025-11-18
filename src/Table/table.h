#ifndef TABLE
#define TABLE
#include "../Ast/ast.h"
/* Simple linked list implementation for a key-value table
 */
struct _entry {
  char *key;
  enum {
    TBL_INT,
    TBL_BOOL,
    TBL_STRING,
    TBL_ID,
    TBL_FUNCTION,
    TBL_ERROR
  } typeTag;
  struct _entry *next;
};

typedef struct _entry Entry;

typedef Entry *Table;

// extern struct type lookup_value(Table, char *);
extern Entry *lookup(Table, char *);
extern Table addEntry(Table, char *, int type);
// extern void update_value(Entry *, struct type);
extern Table addVariableDeclarations(Table, Stm);
extern void printTable(Table);
#endif
