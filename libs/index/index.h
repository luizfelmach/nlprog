#ifndef INDEX_H
#define INDEX_H

#include <map.h>
#include <pair.h>
#include <stdio.h>

typedef struct _index_item *Index_Item;
typedef struct _index_map *Index_Map;
typedef struct _index *Index;

// struct index_item
void index_item_set(Index_Item ii, int freq, double tf_idf);
void index_set_freq(Index_Item ii, int freq);
void index_set_tfidf(Index_Item ii, double tf_idf);
int index_item_freq(Index_Item ii);
double index_item_tfidf(Index_Item ii);
void index_item_show(Index_Item di);

// struct index_map

// struct index
Index index_new();
Index index_load(FILE *file);
void index_add(Index index, char *key1, char *key2);
void index_insert(Index index, char *key);
void index_sort(Index index, data_cmp cmp);
void index_show(Index index);
int index_size(Index index);
Pair index_at(Index index, int pos);
Map index_get(Index index, char *key);
Index_Item index_at_get(Index index, int pos, char *key);
Index_Item index_get_at(Index index, char *key, int pos);
void index_write(Index index, FILE *file);
void index_destroy(Index index);

#endif  // INDEX_H
