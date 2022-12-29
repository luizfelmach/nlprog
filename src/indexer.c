#include <map.h>
#include <math.h>
#include <primitive.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector.h>

int total_docs = 0;

typedef struct {
    int freq;
    double tf_idf;
} Index;

Index *index_new(int freq, double tf_idf);
void index_show(Index *di, void *ctx);

void inverted_index_add(Map map, char *word, char *doc);
void inverted_index_show(void *data, void *ctx);
int inverted_index_sort(const void *data1, const void *data2);
void inverted_index_destroy(void *data);

void forward_index_add(Map map, char *doc, int word_index, Index *di);
void forward_index_show(void *data, void *ctx);
void forward_index_destroy(void *data);

void map_to_vector(void *data, void *ctx);

void get_words(Vector files_train_name, Map map);

double tf(Map forward_index, char *doc, int word_index);
double df(Map inverted_index, char *word);
double idf(Map inverted_index, int total_docs, char *word);
double tf_idf(Map forward_index, Map inverted_index, int total_docs, char *doc,
              char *word, int word_index);

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("error: missing parameters.\n");
        exit(1);
    }

    char file_input_name[2048];
    char file_output_name[2048];

    // Build files name of input and output
    sprintf(file_input_name, "%s/train.txt", argv[1]);
    sprintf(file_output_name, "%s/%s", argv[1], argv[2]);

    printf("info: current folder is '%s'.\n", argv[1]);
    printf("info: path to file input is '%s'.\n", file_input_name);
    printf("info: path to file output is '%s'.\n", file_output_name);

    FILE *file_input = fopen(file_input_name, "r");

    if (!file_input) {
        printf("error: can not open file '%s'.\n", file_input_name);
        exit(1);
    }

    FILE *file_output = fopen(file_output_name, "w");

    if (!file_output) {
        printf("error: can not open file '%s'.\n", file_output_name);
        exit(1);
    }


    Vector files_train_name = vector_new();

    // Get all files name in folder /train
    while (1) {
        char *file_train_name = malloc(sizeof(char) * 1024);
        char temp[1024];
        if (fscanf(file_input, "%s %*s", temp) < 1) {
            free(file_train_name);
            break;
        }
        sprintf(file_train_name, "%s/%s", argv[1], temp);
        vector_push(files_train_name, file_train_name);
    }

    total_docs = vector_size(files_train_name);

    fclose(file_input);

    // map<pair<string, map<pair<string, Document_Index>>>>
    Map inverted_index_map = map_new();
    // map<pair<string, map<pair<string, int>>>>
    Map forward_index_map = map_new();
    // vector<pair<string, map<string, Index>>>
    Vector inverted_index_vector = vector_new();
    // vector<pair<string, map<pair<string, int>>>>
    Vector forward_index_vector = vector_new();

    get_words(files_train_name, inverted_index_map);
    vector_destroy(files_train_name, free);

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

    // Generate tf-idf
    for (i = 0; i < vector_size(forward_index_vector); i++) {
        Pair p = (Pair)vector_at(forward_index_vector, i);
        char *key = (char *)pair_first(p);
        void fn(void *data, void *ctx) {
            Pair k = data;
            char *word_index = pair_first(k);
            Pair po = vector_at(inverted_index_vector, atoi(word_index));
            Index *di = pair_second(k);
            di->tf_idf =
                tf_idf(forward_index_map, inverted_index_map, total_docs, key,
                       (char *)pair_first(po), atoi(word_index));
        }
        map_foreach((Map)pair_second(p), fn, NULL);
    }

    printf("------ INVERTED INDEX ------\n\n");
    vector_foreach(inverted_index_vector, inverted_index_show, file_output);
    printf("\n");
    printf("------ FORWARD INDEX ------\n\n");
    //vector_foreach(forward_index_vector, forward_index_show, NULL);
    fclose(file_output);

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

void index_show(Index *di, void *ctx) {
    fprintf((FILE*)ctx,"freq: %d\n", di->freq);
    fprintf((FILE*)ctx,"tf-idf: %.2lf", di->tf_idf);
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
        fprintf((FILE*)ctx ,"document: %s\n", k); // index
        index_show(di, ctx); //freq tf-idf
        fprintf((FILE*)ctx, "\n\n"); 
    }
    fprintf((FILE*)ctx,"# %s\n", key); // name
    // qtdd de arquivos que essa palavra contem
    map_foreach(value, fn, ctx);
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
        index_show(di,ctx);
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

void get_words(Vector files_train_name, Map map) {
    int i;
    for (i = 0; i < vector_size(files_train_name); i++) {
        char *file_train_name = vector_at(files_train_name, i);
        FILE *file_train = fopen(file_train_name, "r");
        while (1) {
            char *word = malloc(sizeof(char) * 2048);
            if (fscanf(file_train, "%s", word) < 1) {
                free(word);
                break;
            }
            char doc[1024];
            sprintf(doc, "%d", i);
            inverted_index_add(map, word, doc);
            free(word);
        }
        fclose(file_train);
    }
}

double tf(Map forward_index, char *doc, int word_index) {
    Pair p = map_get(forward_index, doc);
    if (!p) {
        printf("error: unexpected error in tf1!\n");
        exit(1);
    }
    Map value = pair_second(p);

    char index[1028];
    sprintf(index, "%d", word_index);

    Pair k = map_get(value, index);
    if (!k) {
        printf("error: unexpected error in tf2!\n");
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
        printf("error: unexpected error in df!\n");
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
