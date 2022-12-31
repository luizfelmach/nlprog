#ifndef INVERTED_INDEX_H
#define INVERTED_INDEX_H

#include <stdio.h>

typedef struct _inverted_index *Inverted_Index;

Inverted_Index inverted_index_new();
Inverted_Index inverted_index_load(FILE *file);
void inverted_index_add(Inverted_Index ii, char *word, int doc);
void inverted_index_show(Inverted_Index ii);
void inverted_index_write(Inverted_Index ii, FILE *file);
void inverted_index_destroy(Inverted_Index ii);

#endif  // INVERTED_INDEX_H
