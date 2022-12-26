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
} Inverted_Index_Item;

Inverted_Index_Item *inverted_index_item_new(int doc, int freq, double tf_idf);
void inverted_index_add(Map map, char *word, int doc);
void inverted_index_show(void *data, void *ctx);
void inverted_index_destroy(void *data);

int main(int argc, char *argv[]) {
    Map inverted_index = map_new();
    int i;
    for (i = 0; i < 4; i++) {
        inverted_index_add(inverted_index, doc0[i], 0);
    }
    for (i = 0; i < 3; i++) {
        inverted_index_add(inverted_index, doc1[i], 1);
    }
    map_foreach(inverted_index, inverted_index_show, NULL);
    map_destroy(inverted_index, free, inverted_index_destroy);
    return 0;
}

Inverted_Index_Item *inverted_index_item_new(int doc, int freq, double tf_idf) {
    Inverted_Index_Item *iii = calloc(1, sizeof(Inverted_Index_Item));
    iii->doc = doc;
    iii->freq = freq;
    iii->tf_idf = tf_idf;
    return iii;
}

void inverted_index_add(Map map, char *word, int doc) {
    Pair p = map_get(map, word);
    if (!p) {
        map_insert(map, new_string(word), vector_new());
        p = map_get(map, word);
    }
    Vector value = pair_second(p);
    Inverted_Index_Item *iii = vector_at(value, doc);
    if (!iii) {
        vector_push(value, inverted_index_item_new(doc, 1, 0));
    } else {
        iii->freq += 1;
    }
}

void inverted_index_show(void *data, void *ctx) {
    Pair p = (Pair)data;
    char *key = pair_first(p);
    Vector value = pair_second(p);
    void fn(void *data, void *ctx) {
        Inverted_Index_Item *iii = data;
        printf("%d %d %.2lf     ", iii->doc, iii->freq, iii->tf_idf);
    }
    printf("%s  ", key);
    vector_foreach(value, fn, NULL);
    printf("\n");
}

void inverted_index_destroy(void *data) {
    Vector value = data;
    vector_destroy(data, free);
}