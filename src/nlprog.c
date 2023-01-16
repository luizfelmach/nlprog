#include <algo.h>
#include <classname_map.h>
#include <index.h>
#include <map.h>
#include <math.h>
#include <primitive.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector.h>

// nlprog

void setup(int argc, char *argv[], Index *inverted, Index *forward, int *k);
void destroy_pair_inside_vector(void *data);
Vector get_words_input(char *label);

// search engine

void search_show_docs(Vector docs_index, Vector tfidf, Index forward);
void search_sum_tfidf(Index inverted, Index forward, Vector words_input,
                      Vector values);
void search_engine(Index inverted, Index forward);

// classifier

void classifier_show(Vector docs_index, Index forward);
void classifier(Index inverted, Index forward, int k);

// doc report

void doc_report_show(Vector docs_index, Vector docs_freq, Index forward);
void doc_report(Index forward);
void show_document(Vector v);

// word report

void word_report_show(Vector docs_index, Vector word_freq, Index inverted,
                      Index forward, char *word);
void word_report(Index inverted, Index forward);

int main(int argc, char *argv[]) {
    int k;
    Index inverted, forward;
    setup(argc, argv, &inverted, &forward, &k);

    //search_engine(inverted, forward);
    classifier(inverted, forward, k);
    //doc_report(forward);
    //word_report(inverted, forward);

    index_destroy(inverted);
    index_destroy(forward);
    return 0;
}

// nlprog

void setup(int argc, char *argv[], Index *inverted, Index *forward, int *k) {
    if (argc < 3) {
        printf("usage: ./nlprog <path> <number>.\n");
        exit(1);
    }
    FILE *file_indexes = fopen(argv[1], "rb");
    if (!file_indexes) {
        printf("error: can not open file '%s'.\n", argv[1]);
        exit(1);
    }
    *k = atoi(argv[2]);
    *inverted = index_load(file_indexes);
    *forward = index_load(file_indexes);
    fclose(file_indexes);
}

void destroy_pair_inside_vector(void *data) {
    pair_destroy(data, free, free);
}

Vector get_words_input(char *label) {
    printf("%s", label);
    Vector words_input = vector_new();
    char *word = NULL;
    size_t _ = 0;
    int len = getline(&word, &_, stdin);
    word[len - 1] = '\0';
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
    printf("\n");

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
        const char *classname = classname_map_get(class);

        printf("# %5d \t", *doc_index);
        printf("%12s \t", classname);
        printf("%.2lf \t", *sum);
        printf("%s\n", path);
    }
}

void search_sum_tfidf(Index inverted, Index forward, Vector words_input,
                      Vector values) {
    char *word_input, doc_index[2048];
    Index_Map im;
    void *_;
    index_for(_, im, forward) {
        double sum = 0;
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
}

void search_engine(Index inverted, Index forward) {
    printf("\n............ search engine ............\n\n");

    Vector words_input = get_words_input("search: ");
    Vector docs_index = vector_new();
    Vector tfidf = vector_new();
    Vector values = vector_new();

    search_sum_tfidf(inverted, forward, words_input, values);
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
    vector_destroy(values, destroy_pair_inside_vector);
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

double magnetude(Vector v) {
    double sum = 0; 
    int *a;
    // double *a;
    vector_for(a, v){
        sum += pow(*a,2);
    }
    return sqrt(sum);
}

double escalar(Vector v1, Vector v2) {
    int *a, *b;
    // double *a, *b;
    double sum = 0;
    vector_for(a, v1) {
        b = vector_at(v2, __i);
        sum += (*a) * (*b);
    }
    return sum;
}

double distance(Vector v1, Vector v2) {
    return escalar(v1,v2)/(magnetude(v1)*magnetude(v2));
}

double calculate_distance_text_to_notice(Index inverted, Index_Map words_index,
                                         int index_doc) {
    Vector tf_idf_text = vector_new();
    Vector tf_idf_notice = vector_new();
    char *word;
    Index_Item di_inverted;
    Index_Item di_words_index;
    Pair p;
    int tf_idf; 
    // double tf_idf;
    double cos = 0;

    map_for(word, di_words_index, words_index) {
        p = index_get_at(inverted, word, index_doc);
        if (p) {
            di_inverted = pair_second(p);
            tf_idf = index_item_freq(di_inverted);
            vector_push(tf_idf_notice, new_int(tf_idf));
            tf_idf = index_item_freq(di_words_index);
            vector_push(tf_idf_text, new_int(tf_idf));
        }
    }

    if(vector_size(tf_idf_notice) < 1){
        return -2;
    }
    cos = distance(tf_idf_text, tf_idf_notice);

    vector_destroy(tf_idf_text, free);
    vector_destroy(tf_idf_notice, free);
    return cos;
}

void classifier(Index inverted, Index forward, int k) {
    if (k > index_size(forward)) {
        printf("warn: k is greater than number of docs.\n");
        return;
    }

    Vector words_input = get_words_input("type the text: ");

    // gerar um indice de palavras ?
    Index_Item ii;
    Index_Map words_index = map_new();
    Vector docs_index = vector_new();
    char *word_input;
    void *_, *__;
    

    vector_for(word_input, words_input) {
        // vai setando a frequencia
        index_map_add(words_index, word_input, 1);
    }

    // teste exibicao, retirar depois
    map_for(word_input, ii, words_index) {
        printf("key: %s \t", word_input);
        index_item_show(ii);
        printf("\n");
    }

    // calcular a distancia
    index_for(_,__, forward){
        double cos = calculate_distance_text_to_notice(inverted, words_index, __i);
        if(cos != -2){
           printf("index: %d, cos %lf\n", __i, cos);
        }
    }

    vector_push(docs_index, new_int(0));
    printf("\nK-Nearest Neighbors - KNN\n\n");
    classifier_show(docs_index, forward);

    map_destroy(words_index, free, free);
    vector_destroy(docs_index, free);
    vector_destroy(words_input, free);
}

// doc report

void doc_report_show(Vector docs_index, Vector docs_freq, Index forward) {
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
        const char *classname = classname_map_get(class);
        int *freq = vector_at(docs_freq, __i);

        printf("# %5d \t", *doc_index);
        printf("%12s \t", classname);
        printf("%d \t", *freq);
        printf("%s\n", path);
    }
}

void doc_report(Index forward) {
    printf("\n............ doc report ............\n\n");

    Vector values = vector_new();
    Vector docs_index_asc = vector_new();
    Vector docs_index_desc = vector_new();
    Vector docs_freq_asc = vector_new();
    Vector docs_freq_desc = vector_new();

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
        vector_push(docs_freq_asc, new_int(*(int *)pair_second(p)));
    }

    vector_sort(values, decrescent_int_sort);
    vector_for(p, values) {
        if (__i > 9) {
            break;
        }
        vector_push(docs_index_desc, new_int(*(int *)pair_first(p)));
        vector_push(docs_freq_desc, new_int(*(int *)pair_second(p)));
    }

    printf("--> asc order\n");
    doc_report_show(docs_index_asc, docs_freq_asc, forward);
    printf("\n--> desc order\n");
    doc_report_show(docs_index_desc, docs_freq_desc, forward);

    vector_destroy(docs_index_asc, free);
    vector_destroy(docs_index_desc, free);
    vector_destroy(docs_freq_asc, free);
    vector_destroy(docs_freq_desc, free);
    vector_destroy(values, destroy_pair_inside_vector);
}

// word report

void word_report_show(Vector docs_index, Vector word_freq, Index inverted,
                      Index forward, char *word) {
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
        const char *classname = classname_map_get(class);
        int *freq = vector_at(word_freq, __i);

        printf("# %5d \t", *doc_index);
        printf("%12s \t", classname);
        printf("%d \t", *freq);
        printf("%s\n", path);
    }
}

void word_report(Index inverted, Index forward) {
    printf("\n............ word report ............\n\n");

    char word[2048];
    printf("query: ");
    scanf("%s%*c", word);
    printf("\n");

    Index_Map im = index_get(inverted, word);
    if (!im) {
        printf("info: '%s' does not appeared.\n", word);
        return;
    }

    Vector docs_index = vector_new();
    Vector word_freq = vector_new();
    Vector values = vector_new();

    printf("info: '%s' appeared in %d/%d docs.\n", word, map_size(im),
           index_size(forward));
    printf("\n");

    Index_Item ii;
    char *doc_index;
    map_for(doc_index, ii, im) {
        Pair p =
            pair_new(new_int(atoi(doc_index)), new_int(index_item_freq(ii)));
        vector_push(values, p);
    }

    vector_sort(values, decrescent_int_sort);

    Pair p;
    vector_for(p, values) {
        if (__i > 9) {
            break;
        }
        vector_push(docs_index, new_int(*(int *)pair_first(p)));
        vector_push(word_freq, new_int(*(int *)pair_second(p)));
    }

    printf("--> most frequently\n");
    word_report_show(docs_index, word_freq, inverted, forward, word);
    printf("\n--> most frequently (class)\n");
    word_report_show(docs_index, word_freq, inverted, forward, word);

    vector_destroy(docs_index, free);
    vector_destroy(word_freq, free);
    vector_destroy(values, destroy_pair_inside_vector);
}