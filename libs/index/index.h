#ifndef INDEX_H
#define INDEX_H

#include <stdio.h>

typedef struct _index *Index;

Index index_new();
Index index_load(FILE *file);
void index_add(Index ii, char *key1, char *key2);
void index_show(Index ii);
void index_write(Index ii, FILE *file);
void index_destroy(Index ii);

#endif  // INDEX_H
