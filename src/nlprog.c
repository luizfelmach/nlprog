#include <algo.h>
#include <index.h>
#include <map.h>
#include <math.h>
#include <primitive.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector.h>

char *siggles[22] = {"at2", "bro", "cid", "cit", "con2", "eco", "ept",
                     "esp", "fam", "imo", "inf", "int",  "mic", "mul",
                     "opi", "poc", "pot", "reg", "sro",  "tav", "tvt"};

char *classes[22] = {"at2       ",           "Qual a Bronca", "Cidades    ",
                     "Ciencia e Tecnologia", "Concursos ",    "Economia   ",
                     "Esportes   ",          "Especial  ",    "Familia   ",
                     "Imoveis    ",          "Informatica",   "Internacional",
                     "Minha Casa",           "Mulher     ",   "Opiniao    ",
                     "Policia    ",          "Politica  ",    "Regional   ",
                     "Sobre Rodas",          "Tudo a Ver ",   "TV Tudo   "};

// nlprog

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
void show_word_report(Index forward, Map values, char *word);
void show_word(int index, char *key, Index_Item ii);

// ...
void show_class(char *siggle);

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

    Index inverted = index_load(file_indexes);
    Index forward = index_load(file_indexes);

    search_engine(inverted, forward);
    //  classifier(inverted, forward, 10);
    word_report(inverted, forward);
    //  doc_report(forward);

    fclose(file_indexes);
    index_destroy(inverted);
    index_destroy(forward);

    return 0;
}

// nlprog

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

// ...

void show_class(char *siggle) {
    if (!siggle) {
        printf("NULL \t ");
    } else {
        int i;
        for (i = 0; i < 21; i++) {
            if (strcmp(siggle, siggles[i]) == 0) {
                printf("%s", classes[i]);
                break;
            }
        }
        if (i == 21) {
            printf("--       ");
        }
    }
}

void show_word(int index, char *key, Index_Item ii) {
    char doc[2048];
    char class[2048];
    sscanf(key, "%[^,],%s", doc, class);

    printf("index: %d \t doc: %s \t ", index, doc);
    show_class(class);
    printf(" \t ");
    index_item_show(ii);
}