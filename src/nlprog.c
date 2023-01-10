#include <algo.h>
#include <index.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int sum_of_words(Map map);
int crescent_words_cmp(const void *d1, const void *d2);
int decrescent_words_cmp(const void *d1, const void *d2);
void show_doc_report(Index index);

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

    // Gerar relatório de documentos a partir da quantidade de palavras
    printf("\n----------- DOCUMENTS REPORT -----------\n\n");
    printf("\n-------------- decrescent --------------\n\n");
    index_sort(forward_index, decrescent_words_cmp);
    show_doc_report(forward_index);

    printf("\n-------------- crescent ----------------\n\n");
    index_sort(forward_index, crescent_words_cmp);
    show_doc_report(forward_index);

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

int crescent_words_cmp(const void *d1, const void *d2) {
    Map m1 = pair_second(*(const Pair *)d1);
    Map m2 = pair_second(*(const Pair *)d2);

    int total_words1 = sum_of_words(m1);
    int total_words2 = sum_of_words(m2);

    return total_words1 > total_words2;
}

int decrescent_words_cmp(const void *d1, const void *d2) {
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
        printf("index %d \t doc: %s \t class: %s \t words: %d\n", i, doc, class,
               sum_of_words(m));
    }
}
