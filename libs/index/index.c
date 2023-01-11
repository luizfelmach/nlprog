#include <algo.h>
#include <index.h>
#include <map.h>
#include <pair.h>
#include <primitive.h>
#include <stdio.h>
#include <string.h>
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
    printf("freq: %d \t tf-idf: %.2lf\n", di->freq, di->tf_idf);
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

Index index_new() {
    Index index = (Index)calloc(1, sizeof(struct _index));
    index->data_map = map_new();
    return index;
}

void index_itens_load(Index index, char *key, FILE *file) {
    int i, size;
    fread(&size, sizeof(int), 1, file);

    Map map_itens = index_get(index, key);

    for (int i = 0; i < size; i++) {
        char key_item[2048];
        int len;

        fread(&len, sizeof(int), 1, file);
        fread(&key_item, sizeof(char), len, file);

        map_insert(map_itens, new_string(key_item), index_item_load(file));
    }
}

Index index_load(FILE *file) {
    Index index = index_new();
    int i, size;
    fread(&size, sizeof(int), 1, file);

    for (i = 0; i < size; i++) {
        int len;
        char key[2048];

        fread(&len, sizeof(int), 1, file);
        fread(&key, sizeof(char), len, file);

        map_insert(index->data_map, new_string(key), map_new());
        index_itens_load(index, key, file);
    }
    return index;
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

void index_item_write(Index_Item di, FILE *file) {
    fwrite(di, 1, sizeof(struct _index_item), file);
}

void index_itens_write(Index index, char *key, FILE *file) {
    int i;
    Map map_itens = index_get(index, key);

    int size = map_size(map_itens);
    fwrite(&size, sizeof(int), 1, file);

    for (int i = 0; i < size; i++) {
        Pair p = map_at(map_itens, i);
        char *key_item = pair_first(p);
        int len = strlen(key_item) + 1;

        fwrite(&len, sizeof(int), 1, file);
        fwrite(key_item, sizeof(char), len, file);

        Index_Item item = map_get(map_itens, key_item);
        index_item_write(item, file);
    }
}

void index_write(Index index, FILE *file) {
    int i;
    int size = index_size(index);
    fwrite(&size, sizeof(int), 1, file);

    for (i = 0; i < size; i++) {
        Pair p = index_at(index, i);
        char *key = pair_first(p);
        int len = strlen(key) + 1;

        fwrite(&len, sizeof(int), 1, file);
        fwrite(key, sizeof(char), len, file);

        index_itens_write(index, key, file);
    }
}


void index_destroy(Index index) {
    map_destroy(index->data_map, free, call(void, (void *data), {
                    map_destroy((Map)data, free, free);
                }));
    free(index);
}

