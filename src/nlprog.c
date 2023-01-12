#include <algo.h>
#include <index.h>
#include <map.h>
#include <primitive.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector.h>

// search engine
Vector get_words();
void search_engine(Index inverted, Index forward);

// classifier

// word report
void words_report(Index inverted, Index forward);
int decrescent_word_freq_cmp(const void *d1, const void *d2);
void show_word_report(Index forward, Map values, char *word);
void show_word(int index, char *key, Index_Item ii);

// doc report
void doc_report(Index index, data_cmp fn);
int decrescent_size_doc_cmp(const void *d1, const void *d2);
int crescent_size_doc_cmp(const void *d1, const void *d2);
void show_document(void *data, void*ctx);

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

    // printf("\n-------------- inverted --------------\n\n");
    // index_show(inverted_index);
    // printf("\n-------------- forward ---------------\n\n");
    // index_show(forward_index);

    

    search_engine(inverted_index, forward_index);

    printf("\n----------- DOCUMENTS REPORT -----------\n\n");
    printf("\n-------------- decrescent --------------\n\n");
    doc_report(forward_index, decrescent_size_doc_cmp);
    printf("\n--------------- crescent ---------------\n\n");
    doc_report(forward_index, crescent_size_doc_cmp);
    printf("\n------------ WORDS REPORT ---------------\n\n");
    words_report(inverted_index, forward_index);

    fclose(file_indexes);
    index_destroy(inverted_index);
    index_destroy(forward_index);
    return 0;
}

Vector get_words() {
    Vector w = vector_new();
    char *word = NULL;
    size_t len = 0;

    printf("Search engine: ");
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

void search_engine(Index inverted, Index forward) {
    Vector words_input = get_words();
    Vector values = vector_new();

    typedef struct {
        int idx;
        double sum;
    } VALUE;

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
            VALUE *v = malloc(1 * sizeof(VALUE));
            v->idx = i;
            v->sum = sum;
            vector_push(values, v);
        }
    }  // acho que sim testa noticias com palagras aleatorias
    data_cmp values_sort = call(int, (const void *d1, const void *d2), {
        const VALUE *v1 = *(VALUE **)d1;
        const VALUE *v2 = *(VALUE **)d2;  // brabissima
        if (v1->sum - v2->sum < 0) {
            return 1;
        } else if (v1->sum - v2->sum > 0) {
            return -1;
        }
        return 0;
    });
    vector_sort(values, values_sort);

    void (*show)(void *data, void *ctx) = call(void, (void *data, void *ctx), {
        printf("%d %.2lf\n", ((VALUE *)data)->idx, ((VALUE *)data)->sum);
    });

    vector_foreach(values, show, NULL);

    vector_destroy(words_input, free);
    vector_destroy(values, free);
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
        sprintf(index, "%d,%s", i, (char*)pair_first(p)); // index e name
        Pair values = pair_new(new_string(index), new_int(sum));
        vector_push(v,values);
    }
    vector_sort(v,fn);
    vector_foreach(v,show_document,NULL);
    vector_destroy(v, call(void, (void *data),{
        pair_destroy(data, free, free);
    }));
}

void show_document(void *data, void*ctx){
    char index[2048];
    char doc[2048];
    char class[2048];
    int *total = pair_second((Pair)data);
    sscanf((char*)pair_first((Pair)data),"%[^,],%[^,],%s",index,doc,class);

    printf("index: %s \t doc: %s \t ", index, doc);
    show_class(class);
    printf("\t words: %d\n", *total);
}


int decrescent_word_freq_cmp(const void *d1, const void *d2) {
    Index_Item i1 = pair_second(*(const Pair *)d1);
    Index_Item i2 = pair_second(*(const Pair *)d2);

    return index_item_freq(i2) - index_item_freq(i1);
}

void show_word(int index, char *key, Index_Item ii){
    char doc[2048];
    char class[2048];
    sscanf(key,"%[^,],%s",doc,class);

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
        show_word(i, pair_first(p2), ii);
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
        printf("Cidade  ");
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
        printf("Familia ");
    } else if (!strcmp(siggle, "imo")) {
        printf("Imoveis ");
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