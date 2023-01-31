#include <algo.h>
#include <classname_map.h>
#include <index.h>
#include <map.h>
#include <math.h>
#include <maths.h>
#include <primitive.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector.h>

// nlprog
void menu();
void setup(int argc, char *argv[], Index *inverted, Index *forward, int *k);
void destroy_pair_inside_vector(void *data);
Vector get_words_input(char *label);
// cria um vetor de classes a partir do indice de documentos dado
void get_class(Index forward, Vector docs_index, Vector vector_classes);

// search engine

void search_show_docs(Vector docs_index, Vector tfidf, Index forward);
void search_sum_tfidf(Index inverted, Index forward, Vector words_input,
                      Vector values);
void search_engine(Index inverted, Index forward);

// classifier
void classifier(Index inverted, Index forward, int k);

// doc report

void doc_report_show(Vector docs_index, Vector docs_freq, Index forward);
void doc_report(Index forward);
void show_document(Vector v);

// word report
void word_report_class_show(Index_Map map_classes);
void word_report_show(Vector docs_index, Vector word_freq, Index inverted,
                      Index forward, char *word);
void word_report(Index inverted, Index forward);

int main(int argc, char *argv[]) {
    int choice = 1;
    int k;
    Index inverted, forward;
    setup(argc, argv, &inverted, &forward, &k);
   
    while(choice){
        menu();
        scanf("%d%*c", &choice);
        switch (choice)
        {
        case 1:
            classifier(inverted, forward, k);
            break;
        case 2:
            word_report(inverted, forward);
            break;
        case 3:
            doc_report(forward);
            break;
        case 4:
            search_engine(inverted, forward);
            break;
        default:
            break;
        }
    }

    index_destroy(inverted);
    index_destroy(forward);
    return 0;
}

// nlprog

void menu(){
    printf("\n................. MENU ................\n\n");
    printf("Options:\n");
    printf("0 - exit\n1 - classifier\n2 - words report\n3 - documents report\n4 - search\n");
    printf("\n.......................................\n\n");   
}

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

void get_class(Index forward, Vector docs_index, Vector vector_classes) {
    int *idx;
    vector_for(idx, docs_index) {
        Pair p = index_at(forward, *idx);
        char *path_class = pair_first(p);
        char class[2048];
        sscanf(path_class, "%*[^,],%s", class);
        vector_push(vector_classes, new_string(class));
    }
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
    printf("<index> \t <classname> \t <sum of tf-idf> \t <path>\n");
    vector_for(doc_index, docs_index) {
        if (__i > 9) {  // exibe apenas até os 10 primeiros
            break;
        }

        double *sum = vector_at(tfidf, __i);
        Pair p = index_at(forward, *doc_index);
        char *path_class = pair_first(p);
        sscanf(path_class, "%[^,],%s", path, class);
        const char *classname = classname_map_get(class);

        printf("# %5d \t", *doc_index);
        printf("%12s \t", classname);
        printf("%16.2lf \t ", *sum);
        printf("%s\n", path);
    }
}

void search_sum_tfidf(Index inverted, Index forward, Vector words_input,
                      Vector values) {
    char *word_input, doc_index[2048];
    Index_Map im;
    void *_;
    index_for(_, im, forward) {  // varre o indice de documentos
        double sum = 0;
        sprintf(doc_index, "%d", __i);

        vector_for(word_input, words_input) {
            Index_Item ii = index_get_get(inverted, word_input, doc_index);
            if (ii) {
                // se a palavra 'word_input' existe nesse documento
                // adiciono à soma o tf-idf dessa palavra contida no documento
                sum += index_item_tfidf(ii);
            }
        }

        // add somente as somas > 0 e seus respectivos indices de doc à 'values'
        if (sum > 0) {
            Pair p = pair_new(new_int(__i), new_double(sum));
            vector_push(values, p);
        }
    }
}

void search_engine(Index inverted, Index forward) {
    printf("\n............ SEARCH ENGINE ............\n\n");
          

    Vector words_input = get_words_input("search: ");
    Vector docs_index = vector_new();
    Vector tfidf = vector_new();
    Vector values = vector_new();

    // para cada documento soma os tf-idf's do documento ∩ 'words_input'
    // o indice e a soma dos tf-idf's sao armazenados em 'values'
    search_sum_tfidf(inverted, forward, words_input, values);
    // ordem decrescente da soma de tf-idf's
    vector_sort(values, decrescent_double_sort);

    // separa os dados do vetor 'values' em 2
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

void classifier(Index inverted, Index forward, int k) {
    printf("\n.............. CLASSIFIER .............\n\n");
         
    if (k > index_size(forward)) {
        printf("warn: k is greater than number of docs.\n");
        return;
    }
    Vector words_input = get_words_input("type the text: ");
    Index_Map typed_news = map_new();
    Index_Map im;
    Index_Item ii;
    int len_docs;
    double tf_idf;
    char *word_input;
    int total_docs = index_size(forward);
    char *doc_index;

    // transforma as palavras digitadas pelo usuario em uma especie de noticia
    
    // seta o frequencia
    vector_for(word_input, words_input) {
        index_map_add(typed_news, word_input, 1);
    }

    // seta  o tf-idf
    map_for(word_input, ii, typed_news) {
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

    // classifica
    const char * class = index_classifier(inverted, forward, typed_news, k);
    
    // imprime o resultado
    printf("\nK-Nearest Neighbors - KNN\n\n");
    
    if (!class) {
        printf("info: no results.\n");
    } else {
        printf("the class is: %s \t ", class);
    }
    
    
    vector_destroy(words_input, free);
    map_destroy(typed_news, free, free);
}

// doc report

void doc_report_show(Vector docs_index, Vector docs_freq, Index forward) {
    char path[2048], class[2048];
    int *doc_index;
    if (!vector_size(docs_index)) {
        printf("info: nothing to show.\n");
        return;
    }
    printf("<index> \t <classname> \t <frequency> \t <path>\n");
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
        printf("%12d \t ", *freq);
        printf("%s\n", path);
    }
}

void doc_report(Index forward) {
    printf("\n.............. DOC REPORT .............\n\n");
          

    Vector values = vector_new();
    Vector docs_index_asc = vector_new();
    Vector docs_index_desc = vector_new();
    Vector docs_freq_asc = vector_new();
    Vector docs_freq_desc = vector_new();

    // para cada arquivo, faz a soma total de suas frequencias
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
        vector_push(docs_index_asc, new_int(*(int *)pair_first(p)));  // index
        vector_push(docs_freq_asc, new_int(*(int *)pair_second(p)));  // freq
    }

    vector_sort(values, decrescent_int_sort);
    vector_for(p, values) {
        if (__i > 9) {
            break;
        }
        vector_push(docs_index_desc, new_int(*(int *)pair_first(p)));  // index
        vector_push(docs_freq_desc, new_int(*(int *)pair_second(p)));  // freq
    }

    printf("-> asc order\n");
    doc_report_show(docs_index_asc, docs_freq_asc, forward);
    printf("\n-> desc order\n");
    doc_report_show(docs_index_desc, docs_freq_desc, forward);

    vector_destroy(docs_index_asc, free);
    vector_destroy(docs_index_desc, free);
    vector_destroy(docs_freq_asc, free);
    vector_destroy(docs_freq_desc, free);
    vector_destroy(values, destroy_pair_inside_vector);
}

// word report
void word_report_class_show(Index_Map map_classes) {
    char *class;
    int *freq;
    printf("<rating> \t <classname> \t <word frequency> \n");
    map_for(class, freq, map_classes) {
        if (__i > 9) {
            break;
        }
        printf("# %5d \t", __i + 1);
        printf("%12s \t", classname_map_get(class));
        printf("%17d\n", *freq);
    }
}

void word_report_show(Vector docs_index, Vector word_freq, Index inverted,
                      Index forward, char *word) {
    char path[2048], class[2048];
    int *doc_index;
    if (!vector_size(docs_index)) {
        printf("info: nothing to show.\n");
        return;
    }
    printf("<index> \t <classname> \t <frequency> \t <path>\n");
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
        printf("%12d \t ", *freq);
        printf("%s\n", path);
    }
}

void word_report(Index inverted, Index forward) {
    printf("\n.............. WORD REPORT ............\n\n");

    char word[2048];
    printf("query: ");
    scanf("%s%*c", word);
    printf("\n");

    Index_Map im = index_get(inverted, word);
    if (!im) {
        printf("info: '%s' does not appeared.\n", word);
        return;
    }

    Vector vector_classes = vector_new();
    Vector docs_index = vector_new();
    Vector word_freq = vector_new();
    Vector values = vector_new();
    Index_Map map_classes;

    printf("info: '%s' appeared in %d/%d docs.\n\n", word, map_size(im),
           index_size(forward));

    // para a palavra buscada 'word' copia os indices de documentos e suas
    // respectivas frequencias para o vetor 'values'
    Index_Item ii;
    char *doc_index;
    map_for(doc_index, ii, im) {
        Pair p =
            pair_new(new_int(atoi(doc_index)), new_int(index_item_freq(ii)));
        vector_push(values, p);
    }

    vector_sort(values, decrescent_int_sort);

    // separa os dados de 'values' em 2 vetores
    Pair p;
    vector_for(p, values) {
        vector_push(docs_index, new_int(*(int *)pair_first(p))); // index
        vector_push(word_freq, new_int(*(int *)pair_second(p))); // freq
    }

    // map de classes e freq ordenado de forma decrescente
    get_class(forward, docs_index, vector_classes);
    map_classes = classname_map_generate_map_of_frequency(vector_classes, word_freq);

    printf("-> most frequently\n");
    word_report_show(docs_index, word_freq, inverted, forward, word);
    printf("\n-> most frequently (class)\n");
    word_report_class_show(map_classes);

    map_destroy(map_classes, free, free);
    vector_destroy(vector_classes, free);
    vector_destroy(docs_index, free);
    vector_destroy(word_freq, free);
    vector_destroy(values, destroy_pair_inside_vector);
}