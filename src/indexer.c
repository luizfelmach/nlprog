#include <algo.h>
#include <index.h>
#include <map.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <vector.h>

int total_docs;

void setup(int argc, char *argv[], Vector *path_docs, Vector *class_docs);
void show(Index inverted, Index forward);
void write(int argc, char *argv[], Index inverted, Index forward);
int ensure_exists_paths(Vector path_docs);

void get_inverted(Index inverted, Vector path_docs);
void get_forward(Index forward, Index inverted, Vector path_docs,
                 Vector class_docs);
void generate_tfidf(Index inverted, Index forward);
int inverted_sort(const void *d1, const void *d2);

int main(int argc, char *argv[]) {
    Vector path_docs, class_docs;
    setup(argc, argv, &path_docs, &class_docs);

    Index inverted = index_new();
    Index forward = index_new();

    get_inverted(inverted, path_docs);
    index_sort(inverted, inverted_sort);
    get_forward(forward, inverted, path_docs, class_docs);
    generate_tfidf(inverted, forward);
    show(inverted, forward);

    vector_destroy(path_docs, free);
    vector_destroy(class_docs, free);

    write(argc, argv, inverted, forward);

    index_destroy(inverted);
    index_destroy(forward);
    return EXIT_SUCCESS;
}

void setup(int argc, char *argv[], Vector *path_docs, Vector *class_docs) {
    if (argc < 3) {
        printf("usage: ./indexer <path> <output>.\n");
        exit(1);
    }
    char filename_input[2048];
    sprintf(filename_input, "%s/train.txt", argv[1]);
    printf("info: current folder is '%s'.\n", argv[1]);
    printf("info: path to file input is '%s'.\n", filename_input);
    FILE *file_input = fopen(filename_input, "r");
    if (!file_input) {
        printf("error: can not open file '%s'.\n", filename_input);
        exit(1);
    }
    *path_docs = vector_new();
    *class_docs = vector_new();
    // carrega os nomes dos arquivos e suas respectivas classes
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
        vector_push(*path_docs, path);
        vector_push(*class_docs, class);
    }
    fclose(file_input);
    // verifica se todos os arquivos existem
    if (!ensure_exists_paths(*path_docs)) {
        printf("error: missing files in '/train'.\n");
        vector_destroy(*path_docs, free);
        vector_destroy(*class_docs, free);
        exit(1);
    }
    total_docs = vector_size(*path_docs);
}

void show(Index inverted, Index forward) {
    // run to show: INDEX_SHOW= ./indexer ...
    if (getenv("INDEX_SHOW")) {
        printf("\n-------------- inverted --------------\n\n");
        index_show(inverted);
        printf("\n-------------- forward --------------\n\n");
        index_show(forward);
    }
    printf("info: %d words.\n", index_size(inverted));
    printf("info: %d docs.\n", index_size(forward));
}

void write(int argc, char *argv[], Index inverted, Index forward) {
    char filename_output[2048];
    sprintf(filename_output, "%s/%s", argv[1], argv[2]);
    FILE *file_output = fopen(filename_output, "wb");
    if (!file_output) {
        printf("error: can not write output in '%s'.", filename_output);
        index_destroy(inverted);
        index_destroy(forward);
        exit(1);
    }
    printf("info: writing in '%s'.\n", filename_output);
    index_write(inverted, file_output);
    index_write(forward, file_output);
    fclose(file_output);
}

int ensure_exists_paths(Vector path_docs) {
    int result = 1;
    char *path;
    vector_for(path, path_docs) {
        FILE *file = fopen(path, "r");
        if (!file) {
            printf("warn: file '%s' does not exists.\n", path);
            result = 0;
            continue;
        }
        fclose(file);
    }
    return result;
}

void get_inverted(Index inverted, Vector path_docs) {
    char word[2048], doc[2048], *path;
    vector_for(path, path_docs) {
        FILE *file_doc = fopen(path, "r");
        while (1) {
            if (fscanf(file_doc, "%s", word) < 1) {
                break;
            }
            sprintf(doc, "%d", __i);
            index_add(inverted, word, doc, 1);
        }
        fclose(file_doc);
    }
}

void insert_keys_into_forward(Index forward, Vector path_docs,
                              Vector class_docs) {
    char key[2048];
    int i;
    for (i = 0; i < vector_size(path_docs); i++) {
        sprintf(key, "%s,%s", (char *)vector_at(path_docs, i),
                (char *)vector_at(class_docs, i));
        index_insert(forward, key);
    }
}

void get_forward(Index forward, Index inverted, Vector path_docs,
                 Vector class_docs) {
    char *doc, *class, *path, word_index[2048], doc_key[2048];
    Index_Map docs;
    Index_Item di;
    insert_keys_into_forward(forward, path_docs, class_docs);
    void *_;
    index_for(_, docs, inverted) {
        sprintf(word_index, "%d", __i);
        map_for(doc, di, docs) {
            class = vector_at(class_docs, atoi(doc));
            path = vector_at(path_docs, atoi(doc));
            sprintf(doc_key, "%s,%s", path, class);
            index_add(forward, doc_key, word_index, index_item_freq(di));
        }
    }
}

void generate_tfidf(Index inverted, Index forward) {
    char word_index[2048], *doc_index;
    Index_Item di_inverted, di_forward;
    Index_Map value;
    double tfidf;
    int len_docs;
    void *_;
    index_for(_, value, inverted) {
        len_docs = map_size(value);
        sprintf(word_index, "%d", __i);
        map_for(doc_index, di_inverted, value) {
            di_forward = index_at_get(forward, atoi(doc_index), word_index);
            tfidf = index_calculate_tfidf(index_item_freq(di_inverted),
                                          len_docs, total_docs);
            index_set_tfidf(di_inverted, tfidf);
            index_set_tfidf(di_forward, tfidf);
        }
    }
}

int inverted_sort(const void *d1, const void *d2) {
    const Pair *p1 = d1;
    const Pair *p2 = d2;
    return strcmp((char *)pair_first(*p1), (char *)pair_first(*p2));
}
