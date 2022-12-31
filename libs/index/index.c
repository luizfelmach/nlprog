#include <algo.h>
#include <index.h>
#include <map.h>
#include <pair.h>
#include <primitive.h>
#include <stdio.h>
#include <vector.h>

struct _index {
    Map data_map;
    Vector data_vector;
};

struct _index_item {
    int freq;
    double tf_idf;
};

typedef struct _index_item *Index_Item;

Index_Item index_item_new(int freq, double tf_idf) {
    Index_Item di = (Index_Item)calloc(1, sizeof(struct _index_item));
    di->freq = freq;
    di->tf_idf = tf_idf;
    return di;
}

Index_Item index_item_load(FILE *file) {
    Index_Item di = index_item_new(0, 0);
    fread(di, 1, sizeof(struct _index_item), file);
    return di;
}

void index_item_show(Index_Item di) {
    printf("freq: %d    ", di->freq);
    printf("tf-idf: %.2lf\n", di->tf_idf);
}

void index_item_write(Index_Item di, FILE *file) {
    fwrite(di, 1, sizeof(struct _index_item), file);
}

Index index_new() {
    Index ii = (Index)calloc(1, sizeof(struct _index));
    ii->data_map = map_new();
    ii->data_vector = vector_new();
    return ii;
}

Index index_load(FILE *file) {
}

void index_add(Index ii, char *key1, char *key2) {
    Pair p = map_get(ii->data_map, key1);
    if (!p) {
        map_insert(ii->data_map, new_string(key1), map_new());
        p = map_get(ii->data_map, key1);
        vector_push(ii->data_vector, p);
    }
    Map value = (Map)pair_second(p);
    Pair k = map_get(value, key2);
    if (!k) {
        map_insert(value, new_string(key2), index_item_new(0, 0));
        k = map_get(value, key2);
    }
    Index_Item di = (Index_Item)pair_second(k);
    di->freq += 1;
}

void index_show(Index ii) {
    data_fn fn = call(void, (void *data, void *ctx), {
        char *k = (char *)pair_first((Pair)data);
        Index_Item v = (Index_Item)pair_second((Pair)data);
        printf("# %s\n", k);
        index_item_show(v);
    });
    int i;
    for (i = 0; i < vector_size(ii->data_vector); i++) {
        Pair p = (Pair)vector_at(ii->data_vector, i);
        char *key = (char *)pair_first(p);
        Map value = (Map)pair_second(p);
        printf("%s\n", key);
        map_foreach(value, fn, NULL);
        printf("\n");
    }
}

void index_write(Index ii, FILE *file) {
}

void index_destroy(Index ii) {
    map_destroy(ii->data_map, free, call(void, (void *data), {
                    map_destroy((Map)data, free, free);
                }));
    vector_destroy(ii->data_vector, do_nothing);
    free(ii);
}
