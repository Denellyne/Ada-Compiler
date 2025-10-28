#ifndef TABLE
#define TABLE
#include "../Ast/ast.h"
/* Simple linked list implementation for a key-value table
 */
struct type {
  union {
    int value;
    char *str;
    Stm stm;
  } type;
  int typeTag;
};
struct _entry {
  char *key;
  struct type entryType;
  struct _entry *next;
};

typedef struct _entry Entry;

typedef Entry *Table;

extern struct type lookup_value(Table, char *);
extern Entry *lookup(Table, char *);
extern Table add_entry(Table, char *, struct type);
extern void update_value(Entry *, struct type);
#endif
