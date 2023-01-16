#ifndef INDEX_H
#define INDEX_H

#include <map.h>
#include <pair.h>
#include <stdio.h>

#define index_for(key, index_map, index)              \
    key = pair_first(index_at(index, 0));             \
    index_map = pair_second(index_at(index, 0));      \
    for (int __i = 0; __i < index_size(index); __i++, \
             key = pair_first(index_at(index, __i)),  \
             index_map = pair_second(index_at(index, __i)))

typedef struct _index_item *Index_Item;
typedef Map Index_Map;
typedef struct _index *Index;

// index_item

// Index_Item index_item_new(int freq, double tf_idf); // Private
void index_item_set(Index_Item ii, int freq, double tf_idf);
void index_set_freq(Index_Item ii, int freq);
void index_set_tfidf(Index_Item ii, double tf_idf);
int index_item_freq(Index_Item ii);
double index_item_tfidf(Index_Item ii);
void index_item_show(Index_Item ii);
// Index_Item index_item_load(FILE *file); // Private
// void index_item_write(Index_Item ii, FILE *file); // Private

// index_map

void index_map_add(Index_Map im, char *key, int value);
void index_map_show(Index_Map im);
// Index_Map index_map_load(FILE *file); // Private
// void index_map_write(Index_Map im, FILE *file); // Private

// index

Index index_new();

void index_add(Index index, char *key1, char *key2, int value);
void index_insert(Index index, char *key);
void index_sort(Index index, data_cmp cmp);
void index_show(Index index);
int index_size(Index index);
double index_calculate_tfidf(int freq_p_in_d, int n_docs_p_appeared,
                             int total_docs);

Pair index_at(Index index, int pos);
Index_Map index_get(Index index, char *key);
Pair index_get_at(Index index, char *key, int pos);
Index_Item index_at_get(Index index, int pos, char *key);
Index_Item index_get_get(Index index, char *key1, char *key2);

Index index_load(FILE *file);
void index_write(Index index, FILE *file);

void index_destroy(Index index);

// sort
int decrescent_double_sort(const void *d1, const void *d2);
int decrescent_int_sort(const void *d1, const void *d2);
int crescent_int_sort(const void *d1, const void *d2);
int decrescent_item_freq_sort(const void *d1, const void *d2);
int alphabetic_sort(const void *d1, const void *d2);
#endif  // INDEX_H
