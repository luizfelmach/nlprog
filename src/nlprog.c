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
void get_class(Index forward, Vector docs_index, Vector vector_classes);

// search engine

void search_show_docs(Vector docs_index, Vector tfidf, Index forward);
void search_sum_tfidf(Index inverted, Index forward, Vector words_input,
                      Vector vector_tfidf);
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

    while (choice) {
        menu();
        scanf("%d%*c", &choice);
        switch (choice) {
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

void menu() {
    printf("\n................. MENU ................\n\n");
    printf("Options:\n");
    printf(
        "0 - exit\n1 - classifier\n2 - words report\n3 - documents report\n4 - "
        "search\n");
    printf("\n.......................................\n\n");
}
/*
 * Inicializa o programa e carrega os arquivos necessários
 */
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
/*
 * Captura as palavras digitadas pelo usuário e as transforma em um vetor de
 * palavras
 */
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

/*
 * Transforma um vetor com indices de documentos em um vetor com suas
 * respectivas classes
 */
void get_class(Index forward, Vector docs_index, Vector vector_classes) {
    int *index;
    vector_for(index, docs_index) {
        Pair p = index_at(forward, *index);
        char *path_class = pair_first(p);
        char class[2048];
        sscanf(path_class, "%*[^,],%s", class);
        vector_push(vector_classes, new_string(class));
    }
}

// search engine
/*
 * Exibe os documentos encontrados pelo buscador
 */
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
/*
 * - Gera um vetor com a soma dos tf-idf de cada documeto:
 * 1 - Procura no indice de documentos, para cada documento, todas as palavras
 * contidas em 'words_input'.
 * 2 - Para cada palavra em 'word_input' encontrada nesse
 * documento, soma-se seu tf-idf.
 * 3 - Ao terminar este ciclo, se a soma for maior
 * que zero, ela é salva no vetor 'values'. Repete este ciclo até que o indice
 * de documentos seja completamente varrido.
 */
void search_sum_tfidf(Index inverted, Index forward, Vector words_input,
                      Vector vector_tfidf) {
    char *word_input, doc_index[2048];
    Index_Map doc_content;
    void *_;

    index_for(_, doc_content, forward) {  // 1
        double sum = 0;
        sprintf(doc_index, "%d", __i);

        vector_for(word_input, words_input) {  // 2
            Index_Item item = index_get_get(inverted, word_input, doc_index);
            if (item) {
                sum += index_item_tfidf(item);
            }
        }

        if (sum > 0) {  // 3
            Pair p = pair_new(new_int(__i), new_double(sum));
            vector_push(vector_tfidf, p);
        }
    }
}

/*
 * Buscador:
 * 1 - Apos capturar a entrada do usuáirio, gera um vetor com as somas de tfidf
 * de cada documento e o ordena em ordem decrescente.
 * 2 - Separa os elementos desse vetor em 2 : um com os indices de cada
 * documento e outro com as suas respectivas somas de tfidf
 * 3 - exibe o resultado
 */
void search_engine(Index inverted, Index forward) {
    printf("\n............ SEARCH ENGINE ............\n\n");

    Vector words_input = get_words_input("search: ");
    Vector docs_index = vector_new();
    Vector tfidf_sum = vector_new();
    Vector values = vector_new();

    search_sum_tfidf(inverted, forward, words_input, values);  // 1
    vector_sort(values, decrescent_double_sort);

    Pair p;
    vector_for(p, values) {  // 2
        if (__i > 9) {
            break;
        }
        vector_push(docs_index, new_int(*(int *)pair_first(p)));
        vector_push(tfidf_sum, new_double(*(double *)pair_second(p)));
    }

    search_show_docs(docs_index, tfidf_sum, forward);  // 3

    vector_destroy(words_input, free);
    vector_destroy(docs_index, free);
    vector_destroy(tfidf_sum, free);
    vector_destroy(values, destroy_pair_inside_vector);
}

// classifier
/*
 * Classificador:
 * 1 - Após capturar o texto digitado pelo usuário, transforma esse texto em um
 * novo documento: Seta sua frequencia e seu tfidf.
 * 2 - Classifica esse novo documento.
 * 3 - Exibe o resultado.
 */
void classifier(Index inverted, Index forward, int k) {
    printf("\n.............. CLASSIFIER .............\n\n");

    if (k > index_size(forward)) {
        printf("warn: k is greater than number of docs.\n");
        return;
    }

    Vector text = get_words_input("type the text: ");
    Index_Map new_doc = map_new();
    Index_Map word_content;
    Index_Item item;
    char *word;
    double tfidf;
    int len_docs;
    int total_docs = index_size(forward);

    // seta a frequencia // 1
    vector_for(word, text) {
        index_map_add(new_doc, word, 1);
    }

    // seta  o tf-idf
    map_for(word, item, new_doc) {
        // traz o conteúdo dessa palavra no índice de palavras
        word_content = index_get(inverted, word);
        if (word_content) {                     // se ela existe
            len_docs = map_size(word_content);  // qtdd de doc que ela aparece
        } else {
            len_docs = 0;
        }

        tfidf =
            index_calculate_tfidf(index_item_freq(item), len_docs, total_docs);
        index_set_tfidf(item, tfidf);
    }

    // classifica // 2
    const char *class = index_classifier(inverted, forward, new_doc, k);

    printf("\nK-Nearest Neighbors - KNN\n\n");  // 3

    if (!class) {
        printf("info: no results.\n");
    } else {
        printf("the class is: %s \t ", class);
    }
    vector_destroy(text, free);
    map_destroy(new_doc, free, free);
}

// doc report
/*
 * Exibe o resuldado do relatório de documentos
 */
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

/*
 * Relatório de documentos:
 * 1 - Para cada arquivo salva  o indice desse documento e a soma total de
 * frequencias, ou seja, a quantidade total de palavras no vetor 'values'
 * 2 - Ordena em ordem crescente e salva os indices do documeto em
 * 'docs_index_asc' e a qtdd total de palavras e, 'docs_freq_asc'
 * 3 - Ordena em ordem decrescente e repete o processo com os vetores
 * 'docs_index_desc' e 'docs_freq_desc'
 * 4 - Exibe o resultado
 */
void doc_report(Index forward) {
    printf("\n.............. DOC REPORT .............\n\n");

    Vector docs_index_asc = vector_new();
    Vector docs_index_desc = vector_new();
    Vector docs_freq_asc = vector_new();
    Vector docs_freq_desc = vector_new();
    Vector values = vector_new();
    Index_Map doc_content;
    Index_Item item;
    Pair p;

    // 1
    void *_;
    index_for(_, doc_content, forward) {
        double sum = 0;
        map_for(_, item, doc_content) {
            sum += index_item_freq(item);
        }
        p = pair_new(new_int(__i), new_int(sum));
        vector_push(values, p);
    }

    vector_sort(values, crescent_int_sort);  // 2
    vector_for(p, values) {
        if (__i > 9) {
            break;
        }
        vector_push(docs_index_asc, new_int(*(int *)pair_first(p)));  // index
        vector_push(docs_freq_asc, new_int(*(int *)pair_second(p)));  // freq
    }

    vector_sort(values, decrescent_int_sort);  // 3
    vector_for(p, values) {
        if (__i > 9) {
            break;
        }
        vector_push(docs_index_desc, new_int(*(int *)pair_first(p)));  // index
        vector_push(docs_freq_desc, new_int(*(int *)pair_second(p)));  // freq
    }

    // 4
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

/*
 * Imprime o resuldado do relatorio de palavras ordenado por classes
 */
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

/*
 * Exibe o relatorio de palavras
 */
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
/*
 * Relatório de palavras:
 * 1 - Busca por uma palavra e verifica se ela existe na base de dados
 * 2 - Salva no vetor 'values' o indice do documento que essa palavra aparece e
 * sua frequencia e ordena em ordem decrescente
 * 3 - Separa o vetor 'values' em dois outros
 * 4 - Gera um vetor de classes a partir do indice de documentos
 * 5 - Gera um map de classes com o nome da classe e a sua respectiva frequencia
 * já ordenado em ordem decrescente
 * 6 - Imprime o resultado
 */
void word_report(Index inverted, Index forward) {
    printf("\n.............. WORD REPORT ............\n\n");

    char word[2048];
    printf("query: ");
    scanf("%s%*c", word);  // 1
    printf("\n");

    Index_Map word_content = index_get(inverted, word);
    if (!word_content) {
        printf("info: '%s' does not appeared.\n", word);
        return;
    }

    Vector vector_classes = vector_new();
    Vector docs_index = vector_new();
    Vector word_freq = vector_new();
    Vector values = vector_new();
    Index_Map map_classes;
    Index_Item item;
    Pair p;
    char *doc_index;

    printf("info: '%s' appeared in %d/%d docs.\n\n", word,
           map_size(word_content), index_size(forward));

    map_for(doc_index, item, word_content) { // 2
        p = pair_new(new_int(atoi(doc_index)), new_int(index_item_freq(item)));
        vector_push(values, p);
    }
    // word report
    vector_sort(values, decrescent_int_sort); 
    vector_for(p, values) { // 3
        vector_push(docs_index, new_int(*(int *)pair_first(p)));  // index
        vector_push(word_freq, new_int(*(int *)pair_second(p)));  // freq
    }

    // word report (class)
    get_class(forward, docs_index, vector_classes); // 4
    map_classes =
        classname_map_generate_map_of_frequency(vector_classes, word_freq); // 5

    printf("-> most frequently\n"); // show
    word_report_show(docs_index, word_freq, inverted, forward, word);
    printf("\n-> most frequently (class)\n");
    word_report_class_show(map_classes);

    map_destroy(map_classes, free, free);
    vector_destroy(vector_classes, free);
    vector_destroy(docs_index, free);
    vector_destroy(word_freq, free);
    vector_destroy(values, destroy_pair_inside_vector);
}