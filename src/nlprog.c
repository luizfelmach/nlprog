#include <map.h>
#include <primitive.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector.h>
#include<stdlib.h>

typedef struct {
    int freq;
    double tf_idf;
} Index;

Index *index_new(int freq, double tf_idf);

void populate_inverted_index(Vector v, void *ctx);
void inverted_index_add(Pair p, char *word, char *doc, int freq, double tf_idf);
void inverted_index_show(void *data, void *ctx);

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("error: missing parameters.\n");
        exit(1);
    }

    char file_indexes_name[2048];

    sprintf(file_indexes_name, "%s/%s", argv[1], argv[2]);

    FILE *file_indexes = fopen(file_indexes_name, "r");

    printf("info: current folder is '%s'.\n", argv[1]);
    printf("info: path to file input is '%s'.\n", file_indexes_name);

    if (!file_indexes) {
        printf("error: can not open file '%s'.\n", file_indexes_name);
        exit(1);
    }
    // vector<pair<string, map<string, Index>>>
    Vector inverted_index_vector = vector_new();

    populate_inverted_index(inverted_index_vector, file_indexes);
    fclose(file_indexes);
    
    vector_foreach(inverted_index_vector, inverted_index_show, NULL);

    vector_destroy(inverted_index_vector, do_nothing);

    return 0;
}

void populate_inverted_index(Vector v, void *ctx) {
    int size_map;
    fread(&size_map, 1, sizeof(int), (FILE *)ctx);  // get size of vector
    for (int i = 0; i < size_map; i++) {
        char word[2048];
        int len;

        fread(&len, 1, sizeof(int), (FILE *)ctx);      // get size of word
        fread(&word, len, sizeof(char), (FILE *)ctx);  // get word

        int size_index;
        fread(&size_index, 1, sizeof(int), (FILE *)ctx);  // get size of indexes
        Pair p = pair_new(new_string(word), map_new());
        for (int j = 0; j < size_index; j++) {
            int freq;
            double tf_idf;
            char doc[1024];
            int k;

            fread(&k, 1, sizeof(int), (FILE *)ctx);     // get index document
            fread(&freq, 1, sizeof(int), (FILE *)ctx);  // get frequency
            fread(&tf_idf, 1, sizeof(double), (FILE *)ctx);  // get tf_idf
            sprintf(doc, "%d", k);
            // add  word doc, frequency and tf_idf
            inverted_index_add(p, word, doc, freq, tf_idf);
            vector_push(v, p);
        }
    }
}

void inverted_index_add(Pair p, char *word, char *doc, int freq,
                        double tf_idf) {
    Map value = pair_second(p);
    Pair k = map_get(value, doc);
    if (!k) {
        map_insert(value, new_string(doc), index_new(freq, tf_idf));
    }
}

Index *index_new(int freq, double tf_idf) {
    Index *di = calloc(1, sizeof(Index));
    di->freq = freq;
    di->tf_idf = tf_idf;
    return di;
}

void inverted_index_show(void *data, void *ctx) {
    Pair p = data;
    char *key = pair_first(p);
    Map value = pair_second(p);
    void fn(void *data, void *ctx) {
        char *k = pair_first((Pair)data);
        Index *di = pair_second((Pair)data);
        printf("document: %s\n", k);
        printf("freq: %d\n", di->freq);
        printf("tf-idf: %.2lf", di->tf_idf);
        printf("\n\n");
    }
    printf("# %s\n", key);
    map_foreach(value, fn, NULL);
    printf("\n");
}