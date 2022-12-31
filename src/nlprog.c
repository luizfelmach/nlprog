#include <stdio.h>

int main() {
    return 0;
}

/* #include <map.h>
#include <primitive.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector.h>

typedef struct {
    int freq;
    double tf_idf;
} Index;

Index *index_new(int freq, double tf_idf);

void populate_inverted_index(Vector v, void *ctx);
void inverted_index_show(void *data, void *ctx);

void populate_forward_index(Vector v, void *ctx);
void forward_index_show(void *data, void *ctx) ;

void map_destroy_index(void *data);
void pair_destroy_index(void *data);

void populate_test(Vector v, void *ctx);

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
    Vector inverted_index_vector = vector_new();
    Vector forward_index_vector = vector_new();

    populate_inverted_index(inverted_index_vector, file_indexes);
    populate_forward_index(forward_index_vector, file_indexes);

    fclose(file_indexes);

    vector_foreach(inverted_index_vector, inverted_index_show, NULL);
    vector_foreach(forward_index_vector, forward_index_show, NULL);

    vector_destroy(inverted_index_vector, pair_destroy_index);
    vector_destroy(forward_index_vector, pair_destroy_index);
    return 0;
}

Index *index_new(int freq, double tf_idf) {
    Index *di = calloc(1, sizeof(Index));
    di->freq = freq;
    di->tf_idf = tf_idf;
    return di;
}

void populate_test(Vector v, void *ctx) {
    for (int i = 0; i < 2000; i++) {
        Pair p = pair_new(new_string("banana"), map_new());
        for (int j = 0; j < 30; j++) {
            char str[10];
            sprintf(str, "teste - %d", j);
            Map m = pair_second(p);
            map_insert(m, new_string(str), index_new(i, i));
        }
        vector_push(v, p);
    }
}

void populate_inverted_index(Vector v, void *ctx) {
    int size_vector;
    fread(&size_vector, 1, sizeof(int), (FILE *)ctx);  // get size of vector

    for (int i = 0; i < size_vector; i++) {
        char word[2048];
        int len;

        fread(&len, 1, sizeof(int), (FILE *)ctx);      // get size of word
        fread(&word, len, sizeof(char), (FILE *)ctx);  // get word

        Pair p = pair_new(new_string(word), map_new());

        int size_index;
        fread(&size_index, 1, sizeof(int), (FILE *)ctx);  // get size of indexes

        for (int j = 0; j < size_index; j++) {
            int freq;
            double tf_idf;
            char doc[1024];
            int k;

            fread(&k, 1, sizeof(int), (FILE *)ctx);     // get index document
            fread(&freq, 1, sizeof(int), (FILE *)ctx);  // get frequency
            fread(&tf_idf, 1, sizeof(double), (FILE *)ctx);  // get tf_idf
            sprintf(doc, "%d", k);

            Map m = pair_second(p);
            map_insert(m, new_string(doc), index_new(freq, tf_idf));
        }
        vector_push(v, p);
    }
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

void populate_forward_index(Vector v, void *ctx) {
    int size_vector;
    fread(&size_vector, 1, sizeof(int), (FILE *)ctx);  // get size of vector
    for (int i = 0; i < size_vector; i++) {
        char doc[2048];
        int len;

        fread(&len, 1, sizeof(int), (FILE *)ctx);     // get size of name doc
        fread(&doc, len, sizeof(char), (FILE *)ctx);  // get doc

        Pair p = pair_new(new_string(doc), map_new());

        int size_index;
        fread(&size_index, 1, sizeof(int), (FILE *)ctx);


        for (int j = 0; j < size_index; j++) {
            int freq;
            double tf_idf;
            char idx_word[1024];
            int k;

            fread(&k, 1, sizeof(int), (FILE *)ctx);     // get index document
            fread(&freq, 1, sizeof(int), (FILE *)ctx);  // get frequency
            fread(&tf_idf, 1, sizeof(double), (FILE *)ctx);  // get tf_idf
            sprintf(idx_word, "%d", k);

            Map m = pair_second(p);
            map_insert(m, new_string(idx_word), index_new(freq, tf_idf));
        }
        printf("\n\n");
        vector_push(v, p);
    }
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
        printf("freq: %d\n", di->freq);
        printf("tf-idf: %.2lf", di->tf_idf);
        printf("\n\n");
    }
    map_foreach(value, fn, NULL);
    printf("\n");
}

void map_destroy_index(void *data) {
    Map map = (Map)data;
    map_destroy(map, free, free);
}

void pair_destroy_index(void *data) {
    Pair p = (Pair)data;
    pair_destroy(p, free, map_destroy_index);
} */