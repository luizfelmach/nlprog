#include <algo.h>
#include <index.h>
#include <map.h>
#include <pair.h>
#include <primitive.h>
#include <stdio.h>
#include <vector.h>

struct _index_item {
    int freq;
    double tf_idf;
};

struct _index {
    Map data_map;
};

Index_Item index_item_new(int freq, double tf_idf) {
    Index_Item di = (Index_Item)calloc(1, sizeof(struct _index_item));
    di->freq = freq;
    di->tf_idf = tf_idf;
    return di;
}

void index_item_set(Index_Item ii, int freq, double tf_idf) {
    ii->freq = freq;
    ii->tf_idf = tf_idf;
}

void index_set_freq(Index_Item ii, int freq) {
    ii->freq = freq;
}

void index_set_tfidf(Index_Item ii, double tf_idf) {
    ii->tf_idf = tf_idf;
}

int index_item_freq(Index_Item ii) {
    return ii->freq;
}

double index_item_tfidf(Index_Item ii) {
    return ii->tf_idf;
}

Index_Item index_item_load(FILE *file) {
    Index_Item di = index_item_new(0, 0);
    fread(di, 1, sizeof(struct _index_item), file);
    return di;
}

void index_item_show(Index_Item di) {
    printf("freq: %d     ", di->freq);
    printf("tf-idf: %.2lf\n", di->tf_idf);
}

Pair index_at(Index index, int pos) {
    return (Pair)map_at(index->data_map, pos);
}

void index_sort(Index index, data_cmp cmp) {
    map_sort(index->data_map, cmp);
}

Index_Item index_get_at(Index index, char *key, int pos) {
    Map value = map_get(index->data_map, key);
    if (!value) {
        return NULL;
    }
    return (Index_Item)map_at(value, pos);
}

Map index_get(Index index, char *key) {
    return (Map)map_get(index->data_map, key);
}

Index_Item index_at_get(Index index, int pos, char *key) {
    Pair p = (Pair)map_at(index->data_map, pos);
    if (!p) {
        return NULL;
    }
    Map value = (Map)pair_second(p);
    Index_Item k = map_get(value, key);
    return k;
}

Index_Item index_get_get(Index index, char *key1, char *key2) {
    Map p = map_get(index->data_map, key1);
    if (!p) {
        return NULL;
    }
    Index_Item k = map_get(p, key2);
    return k;
}

void index_item_write(Index_Item di, FILE *file) {
    fwrite(di, 1, sizeof(struct _index_item), file);
}

Index index_new() {
    Index index = (Index)calloc(1, sizeof(struct _index));
    index->data_map = map_new();
    return index;
}

Index index_load(FILE *file) {
    // Todo
}

void index_add(Index index, char *key1, char *key2) {
    Map p = map_get(index->data_map, key1);
    if (!p) {
        map_insert(index->data_map, new_string(key1), map_new());
        p = map_get(index->data_map, key1);
    }
    Index_Item di = map_get(p, key2);
    if (!di) {
        map_insert(p, new_string(key2), index_item_new(0, 0));
        di = map_get(p, key2);
    }
    di->freq += 1;
}

void index_insert(Index index, char *key) {
    Map p = map_get(index->data_map, key);
    if (!p) {
        map_insert(index->data_map, new_string(key), map_new());
    }
}

void index_show(Index index) {
    data_fn fn = call(void, (void *data, void *ctx), {
        char *k = (char *)pair_first((Pair)data);
        Index_Item v = (Index_Item)pair_second((Pair)data);
        printf("# %s     ", k);
        index_item_show(v);
    });
    int i;
    for (i = 0; i < map_size(index->data_map); i++) {
        Pair p = (Pair)map_at(index->data_map, i);
        char *key = (char *)pair_first(p);
        Map value = (Map)pair_second(p);
        printf("%s\n", key);
        map_foreach(value, fn, NULL);
        printf("\n");
    }
}

int index_size(Index index) {
    return map_size(index->data_map);
}

void index_write(Index index, FILE *file) {
    // Todo
}

void index_destroy(Index index) {
    map_destroy(index->data_map, free, call(void, (void *data), {
                    map_destroy((Map)data, free, free);
                }));
    free(index);
}
