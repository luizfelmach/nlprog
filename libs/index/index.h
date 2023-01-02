#ifndef INDEX_H
#define INDEX_H

#include <pair.h>
#include <stdio.h>

typedef struct _index_item *Index_Item;

void index_item_set(Index_Item ii, int freq, double tf_idf);
int index_item_freq(Index_Item ii);
double index_item_tfidf(Index_Item ii);

typedef struct _index *Index;

Index index_new();
Index index_load(FILE *file);
void index_add(Index index, char *key1, char *key2);
void index_show(Index index);
int index_size(Index index);
Pair index_at(Index index, int pos);
Pair index_get(Index index, char *key);
Index_Item index_at_get(Index index, int pos, char *key);
Index_Item index_get_get(Index index, char *key1, char *key2);
void index_write(Index index, FILE *file);
void index_destroy(Index index);

#endif  // INDEX_H
