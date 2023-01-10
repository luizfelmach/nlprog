#include <algo.h>
#include <index.h>
#include <map.h>
#include <math.h>
#include <stdio.h>
#include <vector.h>

int total_docs;

void get_inverted(Index inverted, Vector path_docs);
void get_forward(Index forward, Index inverted, Vector path_docs,
                 Vector class_docs);
void generate_tfidf(Index inverted, Index forward);
double calculate_tfidf(int freq_p_in_d, int n_docs_p_appeared);
int inverted_sort(const void *d1, const void *d2);

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("error: missing parameters.\n");
        exit(1);
    }

    char filename_input[2048];
    char filename_output[2048];

    sprintf(filename_input, "%s/train.txt", argv[1]);
    sprintf(filename_output, "%s/%s", argv[1], argv[2]);

    printf("info: current folder is '%s'.\n", argv[1]);
    printf("info: path to file input is '%s'.\n", filename_input);
    printf("info: path to file output is '%s'.\n", filename_output);

    FILE *file_input = fopen(filename_input, "r");

    if (!file_input) {
        printf("error: can not open file '%s'.\n", filename_input);
        exit(1);
    }

    Vector path_docs = vector_new();
    Vector class_docs = vector_new();

    while (1) {
        char temp1[1024], temp2[1024];
        char *path = malloc(sizeof(char) * 1024);
        char *class = malloc(sizeof(char) * 1024);
        if (fscanf(file_input, "%s %s%*c", temp1, temp2) < 1) {
            free(path);
            free(class);
            break;
        }
        sprintf(path, "%s/%s", argv[1], temp1);
        sprintf(class, "%s", temp2);
        vector_push(path_docs, path);
        vector_push(class_docs, class);
    }
    fclose(file_input);

    total_docs = vector_size(path_docs);

    Index inverted = index_new();
    Index forward = index_new();

    get_inverted(inverted, path_docs);
    index_sort(inverted, inverted_sort);
    get_forward(forward, inverted, path_docs, class_docs);
    generate_tfidf(inverted, forward);

    vector_destroy(path_docs, free);
    vector_destroy(class_docs, free);

    printf("\n-------------- inverted --------------\n\n");
    index_show(inverted);
    printf("\n-------------- forward --------------\n\n");
    index_show(forward);

    FILE *file_output = fopen(filename_output, "wb");

    if (!file_output) {
        printf("error: can not write output in '%s'.", filename_output);
        index_destroy(inverted);
        index_destroy(forward);
        exit(1);
    }

    index_write(inverted, file_output);
    index_write(forward, file_output);

    printf("info: ending and closing program.\n");
    index_destroy(inverted);
    index_destroy(forward);
    return 0;
}

void get_inverted(Index inverted, Vector path_docs) {
    char word[2048], doc[2048], *path;
    FILE *file_doc;
    int i;
    for (i = 0; i < vector_size(path_docs); i++) {
        path = (char *)vector_at(path_docs, i);
        file_doc = fopen(path, "r");
        while (1) {
            if (fscanf(file_doc, "%s", word) < 1) {
                break;
            }
            sprintf(doc, "%d", i);
            index_add(inverted, word, doc);
        }
        fclose(file_doc);
    }
}

void get_forward(Index forward, Index inverted, Vector path_docs,
                 Vector class_docs) {
    char *doc, *class, *path, word_index[2048], doc_key[2048];
    Pair p;
    Map docs;
    Index_Item di;
    int i, j, k;
    for (i = 0; i < vector_size(path_docs); i++) {
        char key[2048];
        sprintf(key, "%s-%s", vector_at(path_docs, i),
                vector_at(class_docs, i));
        index_insert(forward, key);
    }
    for (i = 0; i < index_size(inverted); i++) {
        p = index_at(inverted, i);
        docs = (Map)pair_second(p);
        sprintf(word_index, "%d", i);
        for (j = 0; j < map_size(docs); j++) {
            doc = (char *)pair_first(map_at(docs, j));
            di = (Index_Item)pair_second(map_at(docs, j));
            class = (char *)vector_at(class_docs, atoi(doc));
            path = (char *)vector_at(path_docs, atoi(doc));
            sprintf(doc_key, "%s-%s", path, class);
            for (k = 0; k < index_item_freq(di); k++) {
                index_add(forward, doc_key, word_index);
            }
        }
    }
}

void generate_tfidf(Index inverted, Index forward) {
    char word_index[2048], *doc_index;
    Pair p1, p2;
    Index_Item di_inverted, di_forward;
    Map value;
    double tfidf;
    int i, j, len_docs;
    for (i = 0; i < index_size(inverted); i++) {
        p1 = index_at(inverted, i);
        value = pair_second(p1);
        sprintf(word_index, "%d", i);
        len_docs = map_size(value);
        for (j = 0; j < map_size(value); j++) {
            p2 = map_at(value, j);
            doc_index = pair_first(p2);
            di_inverted = pair_second(p2);
            di_forward = index_at_get(forward, atoi(doc_index), word_index);
            tfidf = calculate_tfidf(index_item_freq(di_inverted), len_docs);
            index_set_tfidf(di_inverted, tfidf);
            index_set_tfidf(di_forward, tfidf);
        }
    }
}

double calculate_tfidf(int freq_p_in_d, int n_docs_p_appeared) {
    double tfidf;
    tfidf = log((double)(1 + total_docs) / (double)(1 + n_docs_p_appeared));
    tfidf += 1;
    tfidf *= freq_p_in_d;
    return tfidf;
}

int inverted_sort(const void *d1, const void *d2) {
    // Todo
}
