#include <map.h>
#include <primitive.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector.h>

char *doc0[] = {"gol", "gol", "time", "gol"};
char *doc1[] = {"carro", "gol", "roubado"};

typedef struct {
    int freq;
    double tf_idf;
} Document_Index;

Document_Index *document_index_new(int freq, double tf_idf);

void inverted_index_add(Map map, char *word, char *doc);
void inverted_index_show(void *data, void *ctx);
int inverted_index_sort(const void *data1, const void *data2);
void inverted_index_destroy(void *data);

void forward_index_add(Map map, char *doc, int word_index, int freq);
void forward_index_show(void *data, void *ctx);
void forward_index_destroy(void *data);

void map_to_vector(void *data, void *ctx);

int main(int argc, char *argv[]) {
    // map<pair<string, map<pair<string, Document_Index>>>>
    Map inverted_index_map = map_new();

    // map<pair<string, map<pair<string, int>>>>
    Map forward_index_map = map_new();

    // vector<pair<string, map<string, Document_Index>>>
    Vector inverted_index_vector = vector_new();

    // vector<pair<string, map<pair<string, int>>>>
    Vector forward_index_vector = vector_new();

    int i;
    for (i = 0; i < 4; i++) {
        inverted_index_add(inverted_index_map, doc0[i], "0");
    }
    for (i = 0; i < 3; i++) {
        inverted_index_add(inverted_index_map, doc1[i], "1");
    }

    map_foreach(inverted_index_map, map_to_vector, inverted_index_vector);
    vector_sort(inverted_index_vector, inverted_index_sort);

    for (i = 0; i < vector_size(inverted_index_vector); i++) {
        Pair p = vector_at(inverted_index_vector, i);
        char *key = pair_first(p);
        Map value = pair_second(p);
        void fn(void *data, void *ctx) {
            Pair p = data;
            int *v = pair_second(p);
            forward_index_add(forward_index_map, (char *)pair_first(p), i, *v);
        }
        map_foreach(value, fn, NULL);
    }

    map_foreach(forward_index_map, map_to_vector, forward_index_vector);

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

Document_Index *document_index_new(int freq, double tf_idf) {
    Document_Index *di = calloc(1, sizeof(Document_Index));
    di->freq = freq;
    di->tf_idf = tf_idf;
    return di;
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
        map_insert(value, new_string(doc), document_index_new(0, 0));
        k = map_get(value, doc);
    }
    Document_Index *di = pair_second(k);
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
        Document_Index *di = pair_second((Pair)data);
        printf("document: %s\nfreq: %d\ntf-idf: %.2lf\n\n", k, di->freq,
               di->tf_idf);
    }
    printf("# %s\n", key);
    map_foreach(value, fn, NULL);
    printf("\n");
}

void inverted_index_destroy(void *data) {
    Map value = data;
    map_destroy(data, free, free);
}

void forward_index_add(Map map, char *doc, int word_index, int freq) {
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
        map_insert(value, new_string(key), new_int(0));
        k = map_get(value, key);
    }
    int *v = pair_second(k);
    *v += freq;
}

void forward_index_show(void *data, void *ctx) {
    Pair p = data;
    char *key = pair_first(p);
    Map value = pair_second(p);
    printf("# document %s\n", key);
    void fn(void *data, void *ctx) {
        Pair p = data;
        char *k = pair_first(p);
        int *v = pair_second(p);
        printf("word: %s\nfreq: %d\n\n", k, *v);
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