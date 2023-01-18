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
double classifier_distance(Index inverted, Index_Map words_index,
                           char *index_doc, Index_Map document);
void classifier_show(Vector docs_index, Vector similarity, Index forward, const char *classname);
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

    // search_engine(inverted, forward);
    classifier(inverted, forward, k);
    // doc_report(forward);
    // word_report(inverted, forward);

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
            if (ii) {  // for all words from words_input found inside a document
                       // __i
                sum += index_item_tfidf(ii);  // sum it's tf-idfs
            }
        }

        if (sum > 0) {  // sum = 0 is invalid
            Pair p = pair_new(new_int(__i),
                              new_double(sum));  // index and tf-idf's sum
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
    vector_sort(values, decrescent_double_sort);  // decrescent order of sum of
                                                  // values from tf-idf's

    Pair p;
    vector_for(p, values) {
        if (__i > 9) {
            break;
        }
        // splits the data into two vectors
        vector_push(docs_index, new_int(*(int *)pair_first(p)));
        vector_push(tfidf, new_double(*(double *)pair_second(p)));
    }

    // show
    search_show_docs(docs_index, tfidf, forward);

    vector_destroy(words_input, free);
    vector_destroy(docs_index, free);
    vector_destroy(tfidf, free);
    vector_destroy(values, destroy_pair_inside_vector);
}

// classifier

void classifier_show(Vector docs_index, Vector similarity, Index forward, const char *classname) {
    char path[2048], class[2048];
    int *doc_index;

    printf("\nK-Nearest Neighbors - KNN\n\n");
    if (!vector_size(docs_index)) {
        printf("info: no results.\n");
        return;
    } else {
        printf("The class is: %s\n\n", classname);
    }
    vector_for(doc_index, docs_index) {
        double *cos = vector_at(similarity, __i);
        // the document of that position in the index of documents
        Pair p = index_at(forward, *doc_index);
        char *path_class = pair_first(p);
        sscanf(path_class, "%[^,],%s", path, class);
        printf("path: %s \t class: %s \t similarity: %.2lf%%\n", path, classname_map_get(class), (*cos) * 100);
    }
}

double classifier_distance(Index inverted, Index_Map words_index,
                           char *index_doc, Index_Map document) {
    Vector tf_idf_text = vector_new();
    Vector tf_idf_notice = vector_new();
    Index_Item di_words_index;
    Index_Item di_document;
    char *word;
    double tf_idf;
    double cos = 0;

    map_for(word, di_document, document) {
        Pair p = index_at(inverted, atoi(word));
        char *key = pair_first(p);

        di_words_index = map_get(words_index, key);
        if (di_words_index) {
            tf_idf = index_item_tfidf(di_words_index);
        } else {
            tf_idf = 0;
        }
        vector_push(tf_idf_text, new_double(tf_idf));
        tf_idf = index_item_tfidf(di_document);
        vector_push(tf_idf_notice, new_double(tf_idf));
    }

    // se nao existe nenhuma palavra em comum
    double *i, count = 0;
    vector_for(i, tf_idf_text) {
        if (*i != 0.0) {
            count++;
        }
    }
    if (!count) {
        vector_destroy(tf_idf_text, free);
        vector_destroy(tf_idf_notice, free);
        return 0;
    }

    // calculates the distance between the two vectors
    cos = distance(tf_idf_text, tf_idf_notice);

    vector_destroy(tf_idf_text, free);
    vector_destroy(tf_idf_notice, free);
    return cos;
}

void get_words_index(Index inverted, Index_Map words_index, Vector words_input,
                     int total_docs) {
    Index_Map im;
    Index_Item ii;
    int len_docs;
    double tf_idf;
    char *word_input;

    // set frequency
    vector_for(word_input, words_input) {
        index_map_add(words_index, word_input, 1);
    }

    // set tf-idf
    map_for(word_input, ii, words_index) {
        im = index_get(inverted, word_input);
        if (im) {
            len_docs = map_size(im);
        } else {
            len_docs = 0;
        }
        tf_idf =
            index_calculate_tfidf(index_item_freq(ii), len_docs, total_docs);
        index_set_tfidf(ii, tf_idf);
    }
}

const char *calculate_the_class_most_frequently(Index forward,
                                                Vector docs_index) {
    Index_Map classes_map = map_new();
    Pair p;
    char key[2048];
    int *idx;

    vector_for(idx, docs_index) {
        
        // get class
        p = index_at(forward, *idx);
        char *path_class = pair_first(p);
        char class[2048];
        sscanf(path_class, "%*[^,],%s", class);

        // insert class
        int *freq = map_get(classes_map, class);
        if (!freq) {
            map_insert(classes_map, new_string(class), new_int(0));
            freq = map_get(classes_map, class);
        }
        *freq += 1;
    }

    map_sort(classes_map, decrescent_int_sort);

    // get the first element from 
    p = map_at(classes_map, 0);
    sscanf((char *)pair_first(p), "%s", key);
    map_destroy(classes_map, free, free);

    return classname_map_get(key);
}

void classifier(Index inverted, Index forward, int k) {
    if (k > index_size(forward)) {
        printf("warn: k is greater than number of docs.\n");
        return;
    }

    Vector words_input = get_words_input("type the text: ");
    Vector docs_index = vector_new();
    Vector cosseno = vector_new();
    Vector values = vector_new();
    Index_Map words_index = map_new();
    Index_Map im;
    Pair p;
    char index_doc[2048];
    void *_;

    // create 'words_index' and set its frequency and its tf-idf
    get_words_index(inverted, words_index, words_input, index_size(forward));

    // calculate distance
    index_for(_, im, forward) {
        sprintf(index_doc, "%d", __i);
        double cos = classifier_distance(inverted, words_index, index_doc, im);
        // if (cos) {
        p = pair_new(new_int(__i), new_double(cos));
        vector_push(values, p);
        //}
    }
    // sort
    vector_sort(values, decrescent_double_sort);

    // navigates through the first k positions of 'values'
    vector_for(p, values) {
        if (__i > k) {
            break;
        }
        // get index
        int idx = *(int *)pair_first(p);
        vector_push(docs_index, new_int(idx));
        double cos = *(double*)pair_second(p);
        vector_push(cosseno, new_double(cos));
    }

    const char *key = calculate_the_class_most_frequently(forward, docs_index);

    // show
    classifier_show(docs_index, cosseno , forward, key);

    // destroy
    vector_destroy(values, destroy_pair_inside_vector);
    map_destroy(words_index, free, free);
    vector_destroy(docs_index, free);
    vector_destroy(cosseno, free);
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