#include <algo.h>
#include <index.h>
#include <map.h>
#include <math.h>
#include <primitive.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector.h>

// nlprog

void setup(int argc, char *argv[], Index *inverted, Index *forward);
Vector get_words_input(char *label);

// search engine

void search_show_docs(Vector docs_index, Vector tfidf, Index forward);
void search_engine(Index inverted, Index forward);

// classifier

void classifier_show(Vector docs_index, Index forward);
void classifier(Index inverted, Index forward, int k);

// doc report

void doc_report_show(Vector docs_index, Index forward);
void doc_report(Index forward);
void show_document(Vector v);

// word report

void word_report_show(Vector docs_index, Index inverted, Index forward,
                      char *word);
void word_report(Index inverted, Index forward);

int main(int argc, char *argv[]) {
    Index inverted, forward;
    setup(argc, argv, &inverted, &forward);

    // search_engine(inverted, forward);
    // classifier(inverted, forward, 10);
    // word_report(inverted, forward);
    // doc_report(forward);

    index_destroy(inverted);
    index_destroy(forward);
    return 0;
}

// nlprog

void setup(int argc, char *argv[], Index *inverted, Index *forward) {
    char filename_indexes[2048];
    if (argc < 3) {
        printf("error: missing parameters.\n");
        exit(1);
    }
    sprintf(filename_indexes, "%s/%s", argv[1], argv[2]);
    FILE *file_indexes = fopen(filename_indexes, "rb");
    if (!file_indexes) {
        printf("error: can not open file '%s'.\n", filename_indexes);
        exit(1);
    }
    *inverted = index_load(file_indexes);
    *forward = index_load(file_indexes);
    fclose(file_indexes);
}

Vector get_words_input(char *label) {
    printf("%s", label);
    Vector words_input = vector_new();
    char *word = NULL;
    size_t len = 0;
    int tam = getline(&word, &len, stdin);
    word[tam - 1] = '\0';
    char *token = strtok(word, " ");
    while (token) {
        vector_push(words_input, new_string(token));
        token = strtok(NULL, " ");
    }
    free(word);
    return words_input;
}

// search engine

void search_show_docs(Vector docs_index, Vector tfidf, Index forward) {
    char path[2048], class[2048];
    int *doc_index;
    if (!vector_size(docs_index)) {
        printf("info: the search returned no results.\n");
        return;
    }
    vector_for(doc_index, docs_index) {
        if (__i > 9) {
            break;
        }
        double *sum = vector_at(tfidf, __i);
        Pair p = index_at(forward, *doc_index);
        char *path_class = pair_first(p);
        sscanf(path_class, "%[^,],%s", path, class);
        printf("# %d \t %s \t %s \t %.2lf\n", *doc_index, path, class, *sum);
    }
}

void search_engine(Index inverted, Index forward) {
    Vector words_input = get_words_input("search engine: ");
    Vector values = vector_new();
    Vector docs_index = vector_new();
    Vector tfidf = vector_new();
    char *word_input, doc_index[2048];
    double sum;
    Index_Map im;
    void *_;
    index_for(_, im, forward) {
        sum = 0;
        sprintf(doc_index, "%d", __i);
        vector_for(word_input, words_input) {
            Index_Item ii = index_get_get(inverted, word_input, doc_index);
            if (ii) {
                sum += index_item_tfidf(ii);
            }
        }
        if (sum > 0) {
            Pair p = pair_new(new_int(__i), new_double(sum));
            vector_push(values, p);
        }
    }
    vector_sort(values, decrescent_double_sort);
    Pair p;
    vector_for(p, values) {
        if (__i > 9) {
            break;
        }
        vector_push(docs_index, new_int(*(int *)pair_first(p)));
        vector_push(tfidf, new_double(*(double *)pair_second(p)));
    }
    search_show_docs(docs_index, tfidf, forward);
    vector_destroy(words_input, free);
    vector_destroy(docs_index, free);
    vector_destroy(tfidf, free);
    vector_destroy(values, generic_free2(pair_destroy, free, free));
}

// classifier

void classifier_show(Vector docs_index, Index forward) {
    char path[2048], class[2048];
    int *doc_index;
    if (!vector_size(docs_index)) {
        printf("info: no results.\n");
        return;
    }
    vector_for(doc_index, docs_index) {
        Pair p = index_at(forward, *doc_index);
        char *path_class = pair_first(p);
        sscanf(path_class, "%[^,],%s", path, class);
        printf("path: %s \t class: %s\n", path, class);
    }
}

void classifier(Index inverted, Index forward, int k) {
    if (k > index_size(forward)) {
        printf("warn: k is greater than number of docs.\n");
        return;
    }

    Vector words_input = get_words_input("type the text: ");
    Vector docs_index = vector_new();

    vector_push(docs_index, new_int(0));
    printf("\nK-Nearest Neighbors - KNN\n\n");
    classifier_show(docs_index, forward);

    vector_destroy(docs_index, free);
    vector_destroy(words_input, free);
}

// doc report

void doc_report_show(Vector docs_index, Index forward) {
    char path[2048], class[2048];
    int *doc_index;
    if (!vector_size(docs_index)) {
        printf("info: nothing to show.\n");
        return;
    }
    vector_for(doc_index, docs_index) {
        if (__i > 9) {
            break;
        }
        Pair p = index_at(forward, *doc_index);
        char *path_class = pair_first(p);
        sscanf(path_class, "%[^,],%s", path, class);
        printf("index: %d \t path: %s \t class: %s\n", *doc_index, path, class);
    }
}

void doc_report(Index forward) {
    Vector values = vector_new();

    Vector docs_index_asc = vector_new();
    Vector docs_index_desc = vector_new();

    Index_Map im;
    void *_;
    index_for(_, im, forward) {
        double sum = 0;
        Index_Item ii;
        map_for(_, ii, im) {
            sum += index_item_freq(ii);
        }
        Pair p = pair_new(new_int(__i), new_int(sum));
        vector_push(values, p);
    }

    Pair p;
    vector_sort(values, crescent_int_sort);
    vector_for(p, values) {
        if (__i > 9) {
            break;
        }
        vector_push(docs_index_asc, new_int(*(int *)pair_first(p)));
    }

    vector_sort(values, decrescent_int_sort);
    vector_for(p, values) {
        if (__i > 9) {
            break;
        }
        vector_push(docs_index_desc, new_int(*(int *)pair_first(p)));
    }

    printf("\nasc order\n");
    doc_report_show(docs_index_asc, forward);
    printf("\ndesc order\n");
    doc_report_show(docs_index_desc, forward);

    vector_destroy(docs_index_asc, free);
    vector_destroy(docs_index_desc, free);
    vector_destroy(values, generic_free2(pair_destroy, free, free));
}

// word report

void word_report_show(Vector docs_index, Index inverted, Index forward,
                      char *word) {
    char path[2048], class[2048];
    int *doc_index;
    if (!vector_size(docs_index)) {
        printf("info: nothing to show.\n");
        return;
    }
    vector_for(doc_index, docs_index) {
        if (__i > 9) {
            break;
        }
        Pair p = index_at(forward, *doc_index);
        char *path_class = pair_first(p);
        sscanf(path_class, "%[^,],%s", path, class);
        printf("index: %d \t path: %s \t class: %s\n", *doc_index, path, class);
    }
}

void word_report(Index inverted, Index forward) {
    printf("search a word: ");
    char word[2048];
    scanf("%s%*c", word);
    Index_Map im = index_get(inverted, word);
    if (!im) {
        printf("info: word does not exists.\n");
        return;
    }

    Vector docs_index = vector_new();
    Vector values = vector_new();
    printf("info: '%s' appeared in %d docs.\n", word, map_size(im));
    Index_Item ii;
    char *doc_index;
    map_for(doc_index, ii, im) {
        Pair p =
            pair_new(new_int(atoi(doc_index)), new_int(index_item_freq(ii)));
        vector_push(values, p);
    }
    Pair p;
    vector_sort(values, decrescent_int_sort);
    vector_for(p, values) {
        if (__i > 9) {
            break;
        }
        vector_push(docs_index, new_int(*(int *)pair_first(p)));
    }
    word_report_show(docs_index, inverted, forward, word);
    vector_destroy(docs_index, free);
    vector_destroy(values, generic_free2(pair_destroy, free, free));
}