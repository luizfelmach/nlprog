#include <map.h>
#include <math.h>
#include <primitive.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector.h>

int total_docs = 2;
char *doc0[] = {"gol", "gol", "time", "gol"};
char *doc1[] = {"carro", "gol", "roubado"};

typedef struct {
    int freq;
    double tf_idf;
} Index;

Index *index_new(int freq, double tf_idf);
void index_show(Index *di);

void inverted_index_add(Map map, char *word, char *doc);
void inverted_index_show(void *data, void *ctx);
int inverted_index_sort(const void *data1, const void *data2);
void inverted_index_destroy(void *data);

void forward_index_add(Map map, char *doc, int word_index, Index *di);
void forward_index_show(void *data, void *ctx);
void forward_index_destroy(void *data);

void map_to_vector(void *data, void *ctx);

void populate(Map map);

double tf(Map forward_index, char *doc, int word_index);
double df(Map inverted_index, char *word);
double idf(Map inverted_index, int total_docs, char *word);
double tf_idf(Map forward_index, Map inverted_index, int total_docs, char *doc,
              char *word, int word_index);

int main(int argc, char *argv[]) {
    // map<pair<string, map<pair<string, Index>>>>
    Map inverted_index_map = map_new();
    // map<pair<string, map<pair<string, int>>>>
    Map forward_index_map = map_new();
    // vector<pair<string, map<string, Index>>>
    Vector inverted_index_vector = vector_new();
    // vector<pair<string, map<pair<string, int>>>>
    Vector forward_index_vector = vector_new();

    // Generate inverted index
    populate(inverted_index_map);

    map_foreach(inverted_index_map, map_to_vector, inverted_index_vector);
    vector_sort(inverted_index_vector, inverted_index_sort);

    int i;

    // Generate forward index
    for (i = 0; i < vector_size(inverted_index_vector); i++) {
        Pair p = (Pair)vector_at(inverted_index_vector, i);
        void fn(void *data, void *ctx) {
            Pair p = (Pair)data;
            char *key = (char *)pair_first(p);
            Index *value = pair_second(p);
            forward_index_add(forward_index_map, key, i, value);
        }
        map_foreach((Map)pair_second(p), fn, NULL);
    }

    map_foreach(forward_index_map, map_to_vector, forward_index_vector);

    // Generate tf-idf
    for (i = 0; i < vector_size(inverted_index_vector); i++) {
        Pair p = (Pair)vector_at(inverted_index_vector, i);
        char *key = (char *)pair_first(p);
        void fn(void *data, void *ctx) {
            Pair k = data;
            Index *di = pair_second(k);
            di->tf_idf = tf_idf(forward_index_map, inverted_index_map,
                                total_docs, (char *)pair_first(k), key, i);
        }
        map_foreach((Map)pair_second(p), fn, NULL);
    }

    printf("------ INVERTED INDEX ------\n\n");
    vector_foreach(inverted_index_vector, inverted_index_show, NULL);
    printf("\n");
    printf("------ FORWARD INDEX ------\n\n");
    vector_foreach(forward_index_vector, forward_index_show, NULL);

    map_destroy(inverted_index_map, free, inverted_index_destroy);
    map_destroy(forward_index_map, free, forward_index_destroy);
    vector_destroy(inverted_index_vector, do_nothing);
    vector_destroy(forward_index_vector, do_nothing);

    return 0;
}

Index *index_new(int freq, double tf_idf) {
    Index *di = calloc(1, sizeof(Index));
    di->freq = freq;
    di->tf_idf = tf_idf;
    return di;
}

void index_show(Index *di) {
    printf("freq: %d\n", di->freq);
    printf("tf-idf: %.2lf", di->tf_idf);
}

void inverted_index_add(Map map, char *word, char *doc) {
    Pair p = map_get(map, word);
    if (!p) {
        map_insert(map, new_string(word), map_new());
        p = map_get(map, word);
    }
    Map value = pair_second(p);
    Pair k = map_get(value, doc);
    if (!k) {
        map_insert(value, new_string(doc), index_new(0, 0));
        k = map_get(value, doc);
    }
    Index *di = pair_second(k);
    di->freq += 1;
}

int inverted_index_sort(const void *data1, const void *data2) {
    const Pair *p1 = data1;
    const Pair *p2 = data2;
    return strcmp((char *)pair_first(*p1), (char *)pair_first(*p2));
}

void inverted_index_show(void *data, void *ctx) {
    Pair p = data;
    char *key = pair_first(p);
    Map value = pair_second(p);
    void fn(void *data, void *ctx) {
        char *k = pair_first((Pair)data);
        Index *di = pair_second((Pair)data);
        printf("document: %s\n", k);
        index_show(di);
        printf("\n\n");
    }
    printf("# %s\n", key);
    map_foreach(value, fn, NULL);
    printf("\n");
}

void inverted_index_destroy(void *data) {
    Map value = data;
    map_destroy(data, free, free);
}

void forward_index_add(Map map, char *doc, int word_index, Index *di) {
    Pair p = map_get(map, doc);
    if (!p) {
        map_insert(map, new_string(doc), map_new());
        p = map_get(map, doc);
    }
    Map value = pair_second(p);
    char key[2048];
    sprintf(key, "%d", word_index);
    Pair k = map_get(value, key);
    if (!k) {
        map_insert(value, new_string(key), index_new(0, 0));
        k = map_get(value, key);
    }
    Index *v = pair_second(k);
    v->freq += di->freq;
}

void forward_index_show(void *data, void *ctx) {
    Pair p = data;
    char *key = pair_first(p);
    Map value = pair_second(p);
    printf("# document %s\n", key);
    void fn(void *data, void *ctx) {
        Pair p = data;
        char *k = pair_first(p);
        Index *di = pair_second(p);
        printf("word: %s\n", k);
        index_show(di);
        printf("\n\n");
    }
    map_foreach(value, fn, NULL);
    printf("\n");
}

void map_to_vector(void *data, void *ctx) {
    Pair p = (Pair)data;
    Vector v = (Vector)ctx;
    vector_push(v, p);
}

void forward_index_destroy(void *data) {
    Map value = data;
    map_destroy(data, free, free);
}

void populate(Map map) {
    int i;
    for (i = 0; i < 4; i++) {
        inverted_index_add(map, doc0[i], "0");
    }
    for (i = 0; i < 3; i++) {
        inverted_index_add(map, doc1[i], "1");
    }
}

double tf(Map forward_index, char *doc, int word_index) {
    Pair p = map_get(forward_index, doc);
    if (!p) {
        printf("error: unexpected error!\n");
        exit(1);
    }
    Map value = pair_second(p);

    char index[1028];
    sprintf(index, "%d", word_index);

    Pair k = map_get(value, index);
    if (!k) {
        printf("error: unexpected error!\n");
        exit(1);
    }

    int *freq = pair_second(k);
    return (double)*freq;
}

double idf(Map inverted_index, int total_docs, char *word) {
    double result = log((1 + total_docs) / (1 + df(inverted_index, word)));
    result += 1;
    return result;
}

double df(Map inverted_index, char *word) {
    Pair p = map_get(inverted_index, word);
    if (!p) {
        printf("error: unexpected error!\n");
        exit(1);
    }
    Map value = pair_second(p);
    int count = 0;
    void fn(void *data, void *ctx) {
        *(int *)ctx += 1;
    }
    map_foreach(value, fn, &count);
    return count;
}

double tf_idf(Map forward_index, Map inverted_index, int total_docs, char *doc,
              char *word, int word_index) {
    return tf(forward_index, doc, word_index) *
           idf(inverted_index, total_docs, word);
}
