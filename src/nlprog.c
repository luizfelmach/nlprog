#include <algo.h>
#include <index.h>
#include <map.h>
#include <primitive.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector.h>
#include <math.h>

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

// search engine
Vector get_words();
void search_engine(Index inverted, Index forward);
int decrescent_values_sort(const void *d1, const void *d2);
void show_search_document(Vector v);
// classifier
void classifier(Index inverted, Index forward);
// word report
void words_report(Index inverted, Index forward);
int decrescent_word_freq_cmp(const void *d1, const void *d2);
void show_word_report(Index forward, Map values, char *word);
void show_word(int index, char *key, Index_Item ii);

// doc report
void doc_report(Index index, data_cmp fn);
int decrescent_size_doc_cmp(const void *d1, const void *d2);
int crescent_size_doc_cmp(const void *d1, const void *d2);
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

    Index inverted_index = index_load(file_indexes);
    Index forward_index = index_load(file_indexes);
    printf("\n\n########################################################\n\n");
    printf("\n----------------------- inverted -----------------------\n\n");
    index_show(inverted_index);
    printf("\n----------------------- forward ------------------------\n\n");
    index_show(forward_index);
    printf("\n\n########################################################\n\n");
    search_engine(inverted_index, forward_index);

    printf("\n\n#########################################################\n\n");
    printf("\n------------------- DOCUMENTS REPORT --------------------\n\n");
    printf("\n---------------------- decrescent -----------------------\n\n");
    doc_report(forward_index, decrescent_size_doc_cmp);
    printf("\n----------------------- crescent ------------------------\n\n");
    doc_report(forward_index, crescent_size_doc_cmp);
    printf("\n\n#########################################################\n\n");
    printf("\n--------------------- WORDS REPORT ----------------------\n\n");
    words_report(inverted_index, forward_index);
    printf("\n\n#########################################################\n\n");
    printf("\n---------------------- CLASSIFIER -----------------------\n\n");
    classifier(inverted_index, forward_index);

    fclose(file_indexes);
    index_destroy(inverted_index);
    index_destroy(forward_index);
    return 0;
}

// classifier
void get_index_text(Index index, Vector text) {
    char *word, doc[2048], *path;
    int i;
    for (i = 0; i < vector_size(text); i++) {
        word = vector_at(text, i);
        Index_Item ii = index_get_get(index, word, "0");
        
        index_add(index, word, "0", 1);
    }
}

double calculate_tfidf(int freq_p_in_d, int n_docs_p_appeared) {
    double tfidf;
    tfidf = log((double)(1+1) / (double)(1 + n_docs_p_appeared));
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

int inverted_sort(const void *d1, const void *d2) {
    const Pair *p1 = d1;
    const Pair *p2 = d2;
    return strcmp((char *)pair_first(*p1), (char *)pair_first(*p2));
}

void classifier(Index inverted, Index forward) {
    printf("Type the text: ");
    Vector words_expected = get_words();
    Index index_text = index_new();

    get_index_text(index_text, words_expected);
    index_sort(index_text, inverted_sort);

    generate_tfidf(index_text);
    index_show(index_text);

    index_destroy(index_text);
    vector_destroy(words_expected, free);
}

Vector get_words() {
    Vector w = vector_new();
    char *word = NULL;
    size_t len = 0;

    int tam = getline(&word, &len, stdin);
    word[tam - 1] = '\0';

    char *token = strtok(word, " ");
    while (token) {
        vector_push(w, new_string(token));
        token = strtok(NULL, " ");
    }

    free(word);
    return w;
}

int decrescent_values_sort(const void *d1, const void *d2) {
    double *v1 = pair_second(*(const Pair *)d1);
    double *v2 = pair_second(*(const Pair *)d2);
    if (*v1 - *v2 < 0) {
        return 1;
    } else if (*v1 - *v2 > 0) {
        return -1;
    }
    return 0;
}

void show_search_document(Vector v) {
    int i;
    for (i = 0; i < vector_size(v) && i < 10; i++) {
        Pair p = vector_at(v, i);
        char index[2048];
        char doc[2048];
        char class[2048];
        double *total = pair_second(p);
        sscanf((char *)pair_first(p), "%[^,],%[^,],%s", index, doc, class);

        printf("index: %s \t doc: %s \t ", index, doc);
        show_class(class);
        printf("\t tf-idf total: %.2lf\n", *total);
    }
}

void search_engine(Index inverted, Index forward) {
    printf("Search engine: ");
    Vector words_input = get_words();
    Vector values = vector_new();

    double sum;
    int i, j;
    for (i = 0; i < index_size(forward); i++) {
        Pair p = index_at(forward, i);
        Index_Map im = pair_second(p);

        sum = 0;
        for (j = 0; j < vector_size(words_input); j++) {
            // nesse ponto temos a palavra mas nao temos o indice dela;
            // precisamos do indice pois no Index_Map tem apenas o indice.
            char doc_index[2048];
            sprintf(doc_index, "%d", i);
            char *word = vector_at(words_input, j);

            Index_Item ii = index_get_get(inverted, word, doc_index);

            if (ii) {
                sum += index_item_tfidf(ii);
            }
        }
        if (sum > 0) {
            char index[2048];
            sprintf(index, "%d,%s", i, (char *)pair_first(p));
            Pair VALUE = pair_new(new_string(index), new_double(sum));
            vector_push(values, VALUE);
        }
    }
    vector_sort(values, decrescent_values_sort);

    show_search_document(values);

    vector_destroy(words_input, free);
    vector_destroy(
        values, call(void, (void *data), { pair_destroy(data, free, free); }));
}

int decrescent_size_doc_cmp(const void *d1, const void *d2) {
    int *f1 = pair_second(*(const Pair *)d1);
    int *f2 = pair_second(*(const Pair *)d2);
    return *f2 - *f1;
}

int crescent_size_doc_cmp(const void *d1, const void *d2) {
    int *f1 = pair_second(*(const Pair *)d1);
    int *f2 = pair_second(*(const Pair *)d2);
    return *f1 - *f2;
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

int decrescent_word_freq_cmp(const void *d1, const void *d2) {
    Index_Item i1 = pair_second(*(const Pair *)d1);
    Index_Item i2 = pair_second(*(const Pair *)d2);

    return index_item_freq(i2) - index_item_freq(i1);
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
        map_sort(values, decrescent_word_freq_cmp);
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