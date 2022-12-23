#include <libgen.h>
#include <map.h>
#include <primitive.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector.h>

char path[128];

char *words[10] = {
    "c++", "java", "js",      "python",   "c#",
    "c",   "rust", "haskell", "assembly", "rust",
};

typedef struct {
    int doc, freq;
    double tf_idf;
} WIT;

WIT *wit_new(int doc, int freq, double tf_idf);

void inverted_index_show(void *data, void *ctx);
void inverted_index_value_free(void *data) {
    Vector vec = (Vector)data;
    vector_destroy(vec, free);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("error: missing parameters.");
        exit(1);
    }
    strcpy(path, argv[1]);
    dirname(path);
    printf("current folder context: %s\n", path);
    FILE *train = fopen(argv[1], "r");
    if (!train) {
        printf("error: can not open file.");
        exit(1);
    }
    fclose(train);

    Map inverted_index = map_new();

    for (int i = 0; i < 10; i++) {
        Pair p = map_get(inverted_index, words[i]);

        if (!p) {
            Vector vec = vector_new();
            WIT *new_wit = wit_new(1, 1, 0.4);
            vector_push(vec, new_wit);
            map_insert(inverted_index, new_string(words[i]), vec);
        } else {
            Vector vec = (Vector)pair_second(p);
            WIT *new_wit = wit_new(1, 1, 0.4);
            vector_push(vec, new_wit);
        }
    }

    map_foreach(inverted_index, inverted_index_show, NULL);

    map_destroy(inverted_index, free, inverted_index_value_free);

    return 0;
}

void vec_show(void *data, void *ctx) {
    WIT *wit = (WIT *)data;
    printf("%d %d %f\n", wit->doc, wit->freq, wit->tf_idf);
}

void inverted_index_show(void *data, void *ctx) {
    Pair p = (Pair)data;
    Vector vec = pair_second(p);
    vector_foreach(vec, vec_show, NULL);
}

WIT *wit_new(int doc, int freq, double tf_idf) {
    WIT *wit = (WIT *)calloc(1, sizeof(WIT));
    wit->doc = doc;
    wit->freq = freq;
    wit->tf_idf = tf_idf;
    return wit;
}
