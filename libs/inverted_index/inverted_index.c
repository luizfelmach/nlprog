#include <inverted_index.h>
#include <map.h>
#include <pair.h>
#include <primitive.h>
#include <stdio.h>
#include <vector.h>

struct _inverted_index {
    Map data_map;
    Vector data_vector;
};

struct _index {
    int freq;
    double tf_idf;
};

typedef struct _index *Index;

Index index_new(int freq, double tf_idf) {
    Index di = (Index)calloc(1, sizeof(struct _index));
    di->freq = freq;
    di->tf_idf = tf_idf;
    return di;
}

Index index_load(FILE *file) {
    Index di = index_new(0, 0);
    fread(di, 1, sizeof(struct _index), file);
    return di;
}

void index_show(Index di) {
    printf("freq: %d    ", di->freq);
    printf("tf-idf: %.2lf\n", di->tf_idf);
}

void index_write(Index di, FILE *file) {
    fwrite(di, 1, sizeof(struct _index), file);
}

Inverted_Index inverted_index_new() {
    Inverted_Index ii =
        (Inverted_Index)calloc(1, sizeof(struct _inverted_index));
    ii->data_map = map_new();
    ii->data_vector = vector_new();
    return ii;
}

Inverted_Index inverted_index_load(FILE *file) {
}

void inverted_index_add(Inverted_Index ii, char *word, int doc) {
    Pair p = map_get(ii->data_map, word);
    if (!p) {
        map_insert(ii->data_map, new_string(word), map_new());
        p = map_get(ii->data_map, word);
        vector_push(ii->data_vector, p);
    }
    Map value = (Map)pair_second(p);
    char key[2048];
    sprintf(key, "%d", doc);
    Pair k = map_get(value, key);
    if (!k) {
        map_insert(value, new_string(key), index_new(0, 0));
        k = map_get(value, key);
    }
    Index di = (Index)pair_second(k);
    di->freq += 1;
}

void inverted_index_show(Inverted_Index ii) {
    printf("~~~ Inverted Index ~~~\n\n");
    void fn(void *data, void *ctx) {
        char *key = (char *)pair_first((Pair)data);
        Index value = (Index)pair_second((Pair)data);
        printf("# %s\n", key);
        index_show(value);
    }
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

void inverted_index_save(Inverted_Index ii, FILE *file) {
}

void inverted_index_destroy(Inverted_Index ii) {
    void destroy(void *data) {
        map_destroy((Map)data, free, free);
    }
    map_destroy(ii->data_map, free, destroy);
    vector_destroy(ii->data_vector, do_nothing);
    free(ii);
}
