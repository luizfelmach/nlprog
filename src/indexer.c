#include <map.h>
#include <primitive.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector.h>

char *doc0[] = {"gol", "gol", "time", "gol"};
char *doc1[] = {"carro", "gol", "roubado"};

typedef struct {
    int doc, freq;
    double tf_idf;
} Document_Index;

Document_Index *document_index_new(int doc, int freq, double tf_idf);

void inverted_index_add(Map map, char *word, int doc);
void inverted_index_show(void *data, void *ctx);
void map_to_vector(void *data, void *ctx);
void inverted_index_destroy(void *data);

int main(int argc, char *argv[]) {
    Map inverted_index_map = map_new();
    Vector inverted_index_vector = vector_new();

    int i;
    for (i = 0; i < 4; i++) {
        inverted_index_add(inverted_index_map, doc0[i], 0);
    }
    for (i = 0; i < 3; i++) {
        inverted_index_add(inverted_index_map, doc1[i], 1);
    }

    map_foreach(inverted_index_map, inverted_index_show, NULL);
    map_foreach(inverted_index_map, map_to_vector, inverted_index_vector);

    map_destroy(inverted_index_map, free, inverted_index_destroy);
    vector_destroy(inverted_index_vector, do_nothing);
    return 0;
}

Document_Index *document_index_new(int doc, int freq, double tf_idf) {
    Document_Index *di = calloc(1, sizeof(Document_Index));
    di->doc = doc;
    di->freq = freq;
    di->tf_idf = tf_idf;
    return di;
}

void inverted_index_add(Map map, char *word, int doc) {
    Pair p = map_get(map, word);
    if (!p) {
        map_insert(map, new_string(word), vector_new());
        p = map_get(map, word);
    }
    Vector value = pair_second(p);
    int fn(void *data1, void *data2) {
        int *value = data1;
        Document_Index *i = data2;
        if (i->doc == *value) {
            return 0;
        }
        return 1;
    };
    Document_Index *di = vector_search(value, fn, &doc);
    if (!di) {
        vector_push(value, document_index_new(doc, 1, 0));
    } else {
        di->freq += 1;
    }
}

void inverted_index_show(void *data, void *ctx) {
    Pair p = (Pair)data;
    char *key = pair_first(p);
    Vector value = pair_second(p);
    void fn(void *data, void *ctx) {
        Document_Index *di = data;
        printf("%d %d %.2lf     ", di->doc, di->freq, di->tf_idf);
    }
    printf("%s  ", key);
    vector_foreach(value, fn, NULL);
    printf("\n");
}

void map_to_vector(void *data, void *ctx) {
    Pair p = (Pair)data;
    Vector v = (Vector)ctx;
    vector_push(v, p);
}

void inverted_index_destroy(void *data) {
    Vector value = data;
    vector_destroy(data, free);
}