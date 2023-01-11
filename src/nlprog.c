#include <algo.h>
#include <index.h>
#include <map.h>
#include <primitive.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void show_class(char *siggle);
void doc_report(Index forward);
int crescent_size_doc_cmp(const void *d1, const void *d2);
int decrescent_size_doc_cmp(const void *d1, const void *d2);
int sum_of_words(Map map);
void show_doc_report(Index index);

void words_report(Index inverted, Index forward);
int decrescent_word_freq_cmp(const void *d1, const void *d2);
void show_word_report(Index forward, Map values, char *word);

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

    printf("\n-------------- inverted --------------\n\n");
    index_show(inverted_index);
    printf("\n-------------- forward ---------------\n\n");
    index_show(forward_index);

    doc_report(forward_index);
    words_report(inverted_index, forward_index);

    fclose(file_indexes);
    index_destroy(inverted_index);
    index_destroy(forward_index);
    return 0;
}

int sum_of_words(Map map) {
    int sum = 0;
    data_fn fn = call(void, (void *data, void *ctx), {
        Index_Item di = pair_second((Pair)data);
        *(int *)ctx += index_item_freq(di);
    });
    map_foreach(map, fn, &sum);
    return sum;
}

int crescent_size_doc_cmp(const void *d1, const void *d2) {
    Map m1 = pair_second(*(const Pair *)d1);
    Map m2 = pair_second(*(const Pair *)d2);

    int total_words1 = sum_of_words(m1);
    int total_words2 = sum_of_words(m2);

    return total_words1 > total_words2;
}

int decrescent_size_doc_cmp(const void *d1, const void *d2) {
    Map m1 = pair_second(*(const Pair *)d1);
    Map m2 = pair_second(*(const Pair *)d2);

    int total_words1 = sum_of_words(m1);
    int total_words2 = sum_of_words(m2);

    return total_words1 < total_words2;
}

void show_doc_report(Index index) {
    for (int i = 0; i < index_size(index) && i < 10; i++) {  // show 10º firsts
        Pair p = index_at(index, i);
        Map m = pair_second(p);
        char *token = (char *)pair_first(p);
        char *doc = strsep(&token, "-");
        char *class = strsep(&token, "-");
        printf("index %d \t doc: %s \t class: ", i, doc);
        show_class(class);
        printf(" \t words: %d\n", sum_of_words(m));
    }
}

void doc_report(Index forward) {
    // Gerar relatório de documentos a partir da quantidade de palavras
    printf("\n----------- DOCUMENTS REPORT -----------\n\n");
    printf("\n-------------- decrescent --------------\n\n");
    index_sort(forward, decrescent_size_doc_cmp);
    show_doc_report(forward);

    printf("\n-------------- crescent ----------------\n\n");
    index_sort(forward, crescent_size_doc_cmp);
    show_doc_report(forward);
}

int decrescent_word_freq_cmp(const void *d1, const void *d2) {
    Index_Item i1 = pair_second(*(const Pair *)d1);
    Index_Item i2 = pair_second(*(const Pair *)d2);

    return index_item_freq(i1) < index_item_freq(i2);
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
        char *token = (char *)pair_first(p2);
        char *doc = strsep(&token, "-");
        char *class = strsep(&token, "-");
        printf("index: %d \t doc: %s \t ", idx, doc);
        show_class(class);
        printf(" \t ");
        index_item_show(ii);
    }
}

void words_report(Index inverted, Index forward) {
    printf("\n------------ WORDS REPORT ---------------\n\n");
    printf("Search a word: ");
    char word[2048];
    scanf("%s%*c", word);

    Map values = index_get(inverted, word);

    if (!values) {
        printf("\nWord doesn't exists!!\n\n");
    } else {
        map_sort(values, decrescent_word_freq_cmp);
        show_word_report(forward, values, word);
    }
}

void show_class(char *siggle) {
    if (!siggle) {
        printf("NULL \t ");
    } else if (!strcmp(siggle, "at2")) {
        printf("at2 \t ");  // Todo
    } else if (!strcmp(siggle, "bro")) {
        printf("Qual a Bronca?");
    } else if (!strcmp(siggle, "cid")) {
        printf("Cidade");
    } else if (!strcmp(siggle, "cit")) {
        printf("Ciencia e Tecnologia");
    } else if (!strcmp(siggle, "con2")) {
        printf("Concursos");
    } else if (!strcmp(siggle, "eco")) {
        printf("Economia");
    } else if (!strcmp(siggle, "ept")) {
        printf("Esportes");
    } else if (!strcmp(siggle, "esp")) {
        printf("Especial");
    } else if (!strcmp(siggle, "fam")) {
        printf("Familia");
    } else if (!strcmp(siggle, "imo")) {
        printf("Imoveis");
    } else if (!strcmp(siggle, "inf")) {
        printf("Informatica");
    } else if (!strcmp(siggle, "int")) {
        printf("Internacional");
    } else if (!strcmp(siggle, "mic")) {
        printf("Minha Casa");
    } else if (!strcmp(siggle, "mul")) {
        printf("Mulher");
    } else if (!strcmp(siggle, "opi")) {
        printf("Opiniao");
    } else if (!strcmp(siggle, "poc")) {
        printf("Policia");
    } else if (!strcmp(siggle, "pot")) {
        printf("Politica");
    } else if (!strcmp(siggle, "reg")) {
        printf("Regional");
    } else if (!strcmp(siggle, "sro")) {
        printf("Sobre Rodas");
    } else if (!strcmp(siggle, "tav")) {
        printf("Tudo a Ver");
    } else if (!strcmp(siggle, "tvt")) {
        printf("TV Tudo");
    } else {
        printf("--");
    }
}