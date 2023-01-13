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

Vector get_words_input();

// search engine

void search_show_docs(Vector idx, Index forward);
void search_engine(Index inverted, Index forward);

void show_search_document(Vector v);

// classifier
void classifier(Index inverted, Index forward);

// word report
void words_report(Index inverted, Index forward);
void show_word_report(Index forward, Map values, char *word);
void show_word(int index, char *key, Index_Item ii);

// doc report
void doc_report(Index index, data_cmp fn);
void show_document(Vector v);

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

    // printf("\n\n########################################################\n\n");
    // printf("\n----------------------- inverted -----------------------\n\n");
    // index_show(inverted_index);
    // printf("\n----------------------- forward ------------------------\n\n");
    // index_show(forward_index);
    // printf("\n\n########################################################\n\n");
    // search_engine(inverted_index, forward_index);
    // printf("\n\n#########################################################\n\n");
    // printf("\n------------------- DOCUMENTS REPORT
    // --------------------\n\n"); printf("\n---------------------- decrescent
    // -----------------------\n\n"); doc_report(forward_index,
    // decrescent_int_sort); printf("\n----------------------- crescent
    // ------------------------\n\n"); doc_report(forward_index,
    // crescent_int_sort);
    // printf("\n\n#########################################################\n\n");
    // printf("\n--------------------- WORDS REPORT
    // ----------------------\n\n"); words_report(inverted_index,
    // forward_index);
    // printf("\n\n#########################################################\n\n");
    // printf("\n---------------------- CLASSIFIER
    // -----------------------\n\n"); classifier(inverted_index, forward_index);

    fclose(file_indexes);
    index_destroy(inverted);
    index_destroy(forward);

    return 0;
}

// nlprog

Vector get_words_input() {
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

void search_show_docs(Vector docs_index, Index forward) {
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
        Pair p = index_at(forward, *doc_index);
        char *path_class = pair_first(p);
        sscanf(path_class, "%[^,],%s", path, class);
        printf("index: %d \t path: %s \t class: %s\n", *doc_index, path, class);
    }
}

void search_engine(Index inverted, Index forward) {
    printf("Search engine: ");
    Vector words_input = get_words_input();
    Vector values = vector_new();
    Vector docs_index = vector_new();
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
    }
    search_show_docs(docs_index, forward);
    vector_destroy(words_input, free);
    vector_destroy(docs_index, free);
    vector_destroy(values, generic_free2(pair_destroy, free, free));
}

// classifier

void get_index_text(Index index, Vector text) {
    char *word, doc[2048], *path;
    int i;
    for (i = 0; i < vector_size(text); i++) {
        word = vector_at(text, i);
        index_add(index, word, "0", 1);
    }
}

double calculate_tfidf(int freq_p_in_d, int n_docs_p_appeared) {
    double tfidf;
    tfidf = log((double)(1 + 1) / (double)(1 + n_docs_p_appeared));
    tfidf += 1;
    tfidf *= freq_p_in_d;
    return tfidf;
}

void generate_tfidf(Index inverted) {
    Pair p1, p2;
    Index_Item di;
    Map value;
    double tfidf;
    int i, j, len_docs;
    for (i = 0; i < index_size(inverted); i++) {
        p1 = index_at(inverted, i);
        value = pair_second(p1);

        p2 = map_at(value, 0);
        di = pair_second(p2);
        tfidf = calculate_tfidf(index_item_freq(di), 1);
        index_set_tfidf(di, tfidf);
    }
}

void classifier(Index inverted, Index forward) {
    printf("Type the text: ");
    Vector words_expected = get_words_input();
    // Index_Map index_text = map_new();
    // to do
    // get_index_text(index_text, words_expected);
    // generate_tfidf(index_text);

    // map_destroy()
    vector_destroy(words_expected, free);
}

void doc_report(Index index, data_cmp fn) {
    Vector v = vector_new();
    int i, j;
    for (i = 0; i < index_size(index); i++) {
        Pair p = index_at(index, i);    // nesse documento
        Index_Map im = pair_second(p);  // todas as palavras
        double sum = 0;
        for (j = 0; j < map_size(im); j++) {
            Pair p2 = map_at(im, j);
            Index_Item ii = pair_second(p2);
            sum += index_item_freq(ii);
        }
        char index[2048];
        sprintf(index, "%d,%s", i, (char *)pair_first(p));  // index e name
        Pair values = pair_new(new_string(index), new_int(sum));
        vector_push(v, values);
    }
    vector_sort(v, fn);
    show_document(v);
    vector_destroy(
        v, call(void, (void *data), { pair_destroy(data, free, free); }));
}

void show_document(Vector v) {
    int i;
    for (i = 0; i < vector_size(v) && i < 10; i++) {
        Pair p = vector_at(v, i);
        char index[2048];
        char doc[2048];
        char class[2048];
        int *total = pair_second(p);
        sscanf((char *)pair_first(p), "%[^,],%[^,],%s", index, doc, class);

        printf("index: %s \t doc: %s \t ", index, doc);
        show_class(class);
        printf("\t words: %.d\n", *total);
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

void show_word_report(Index forward, Map values, char *word) {
    printf("\nTotal words: %d\n\n", map_size(values));

    for (int i = 0; i < map_size(values) && i < 10; i++) {
        // get all document indexes and itens of list
        Pair p = map_at(values, i);
        int idx = atoi(pair_first(p));
        Index_Item ii = pair_second(p);

        // finding in forward the name and class of doc
        Pair p2 = index_at(forward, idx);
        show_word(idx, pair_first(p2), ii);
    }
}

void words_report(Index inverted, Index forward) {
    printf("Search a word: ");
    char word[2048];
    scanf("%s%*c", word);

    Map values = index_get(inverted, word);

    if (!values) {
        printf("\nWord doesn't exists!!\n\n");
    } else {
        map_sort(values, decrescent_item_freq_sort);
        show_word_report(forward, values, word);
        // to do lista de frequencia de palavras por classe
    }
}

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