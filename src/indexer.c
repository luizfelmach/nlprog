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

Document_Index *documet_index_new(int doc, int freq, double tf_idf);

void inverted_index_add(Map map, char *word, int doc);
void inverted_index_show(void *data, void *ctx);
void inverted_index_destroy(void *data);

int main(int argc, char *argv[]) {
    Map inverted_index_map = map_new();
    int i;
    for (i = 0; i < 4; i++) {
        inverted_index_add(inverted_index_map, doc0[i], 0);
    }
    for (i = 0; i < 3; i++) {
        inverted_index_add(inverted_index_map, doc1[i], 1);
    }
    map_foreach(inverted_index_map, inverted_index_show, NULL);
    map_destroy(inverted_index_map, free, inverted_index_destroy);
    return 0;
}

Document_Index *inverted_index_item_new(int doc, int freq, double tf_idf) {
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
    Document_Index *di = vector_at(value, doc);
    if (!di) {
        vector_push(value, inverted_index_item_new(doc, 1, 0));
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

void inverted_index_destroy(void *data) {
    Vector value = data;
    vector_destroy(data, free);
}