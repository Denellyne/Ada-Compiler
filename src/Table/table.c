
/* Simple key-value table implemented as a linked list
 */

#include "table.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

/*
 * Lookup a name in table; return a value or exits with error
 */
struct type lookup_value(Table tbl, char *name) {
  Entry *ptr = lookup(tbl, name);
  if (ptr == NULL) {
    fprintf(stderr, "unknown name: %s\n", name);
    exit(-1);
  }
  return ptr->entryType;
}

/* Update the value in an entry
 */
void update_value(Entry *ptr, struct type new_value) {
  ptr->entryType = new_value;
}

/* Add an entry to the begining of a table
 */
Table add_entry(Table tbl, char *name, struct type value) {
  Entry *ptr = malloc(sizeof(Entry));
  ptr->key = name;
  ptr->entryType = value;
  ptr->next = tbl;
  return ptr;
}
