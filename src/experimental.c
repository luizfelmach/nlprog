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

// experimental
void setup(int argc, char **argv, Index *inverted, Index *forward, int *k,
           Vector *path_docs, Vector *class_docs);
int ensure_exists_paths(Vector path_docs);
int str_cmp(const void *d1, const void *d2);
void destroy_map_inside_vector(void *data);

// classificador
Vector read_documents(Index forward, Index inverted, Vector paths);
Index_Map doc_load(Index inverted, Index forward, char *dir);
Vector classifier_all_docs(Index inverted, Index forward, Vector docs, int k);

// matriz de confusao
void matrix_of_confusion(Vector predicted_doc_classes,
                         Vector estimated_doc_classes, char *dir);
void matrix_write(int size, int matrix[size][size], char *dir,
                  double hit_percent, Vector vector_classes);
void matrix_show(int size, int matrix[size][size], double hit_percent,
                 Vector vector_classes);

int main(int argc, char **argv) {
    int k;
    Index inverted, forward;
    Vector path_docs, class_docs, estimated_class;

    setup(argc, argv, &inverted, &forward, &k, &path_docs, &class_docs);

    Vector docs = read_documents(forward, inverted, path_docs);
    estimated_class = classifier_all_docs(inverted, forward, docs, k);
    matrix_of_confusion(class_docs, estimated_class, argv[4]);

    vector_destroy(docs, destroy_map_inside_vector);
    vector_destroy(path_docs, free);
    vector_destroy(class_docs, free);
    vector_destroy(estimated_class, free);
    index_destroy(inverted);
    index_destroy(forward);
    return 0;
}

// experimental

void destroy_map_inside_vector(void *data) {
    map_destroy(data, free, free);
}

int str_cmp(const void *d1, const void *d2) {
    return strcmp((char *)d1, (char *)d2);
}
/*
 * Inicializa o programa e carrega os arquivos necessarios
 */
void setup(int argc, char **argv, Index *inverted, Index *forward, int *k,
           Vector *path_docs, Vector *class_docs) {
    if (argc < 5) {
        printf("usage: <path bin> <path test> <number> <output>.\n");
        exit(1);
    }

    // abre o arquivo de saida
    char filename_output[2048];
    sprintf(filename_output, "%s", argv[4]);
    FILE *file_output = fopen(argv[4], "w");
    if (!file_output) {
        printf("error: can not open file '%s'.\n", argv[4]);
        exit(1);
    }
    fclose(file_output);

    // carrega o binario
    FILE *file_indexes = fopen(argv[1], "rb");
    if (!file_indexes) {
        printf("error: can not open file '%s'.\n", argv[1]);
        exit(1);
    }
    *k = atoi(argv[3]);
    *inverted = index_load(file_indexes);
    *forward = index_load(file_indexes);
    fclose(file_indexes);

    // abre o arquivo de test
    char filename_input[2048];
    sprintf(filename_input, "%s", argv[2]);

    char folder [2048];
    argv[2][strlen(argv[2])- strlen("/test.txt")] = '\0';
    sprintf(folder, "%s", argv[2]);

    printf("info: current folder is '%s'.\n", folder);
    printf("info: path to file input is '%s'.\n", filename_input);


    FILE *file_input = fopen(filename_input, "r");
    if (!file_input) {
        printf("error: can not open file '%s'.\n", filename_input);
        index_destroy(*inverted);
        index_destroy(*forward);
        exit(1);
    }

    // salva os diretorios e as classes em dois vetores diferentes
    *path_docs = vector_new();
    *class_docs = vector_new();
    // carrega os nomes dos arquivos e suas respectivas classes
    while (1) {
        char temp1[1024], temp2[1024];
        char *path = malloc(sizeof(char) * 1024);
        char *class = malloc(sizeof(char) * 1024);
        if (fscanf(file_input, "%s %s%*c", temp1, temp2) < 1) {
            free(path);
            free(class);
            break;
        }
        sprintf(path, "%s/%s", folder, temp1);
        sprintf(class, "%s", classname_map_get(temp2));
        vector_push(*path_docs, path);
        vector_push(*class_docs, class);
    }
    fclose(file_input);

    // verifica se todos os arquivos existem
    if (!ensure_exists_paths(*path_docs)) {
        printf("error: missing files in '/train'.\n");
        vector_destroy(*path_docs, free);
        vector_destroy(*class_docs, free);
        index_destroy(*inverted);
        index_destroy(*forward);
        exit(1);
    }
}
/*
 * Verifica se todas os arquivos existem
 */
int ensure_exists_paths(Vector path_docs) {
    int result = 1;
    char *path;
    vector_for(path, path_docs) {
        FILE *file = fopen(path, "r");
        if (!file) {
            printf("warn: file '%s' does not exists.\n", path);
            result = 0;
            continue;
        }
        fclose(file);
    }
    return result;
}

Vector read_documents(Index forward, Index inverted, Vector paths) {
    Vector docs = vector_new();
    Index_Map new_doc;
    char *path;
    printf("info: reading documents\n");
    vector_for(path, paths) {
        new_doc = doc_load(inverted, forward, path);
        vector_push(docs, new_doc);
    }
    return docs;
}
// classifier
/*
 * Le e calcula a frequencia e o tfidf de um novo documento
 */
Index_Map doc_load(Index inverted, Index forward, char *dir) {
    Index_Map new_doc = map_new();
    Index_Map word_content;
    Index_Item item;
    char *word;
    double tfidf;
    int len_docs;
    int total_docs = index_size(forward);

    // le o documento e seta a frequencia
    FILE *file_doc = fopen(dir, "r");
    char word_input[2048];
    if (!file_doc) {
        printf("Unexpected error when trying to open the file %s\n", dir);
        exit(1);
    }

    while (1) {
        if (fscanf(file_doc, "%s", word_input) < 1) {
            break;
        }
        index_map_add(new_doc, word_input, 1);
    }

    fclose(file_doc);

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

    return new_doc;
}

/*
 * Classifica todos os documetos e exibe uma barra de progresso
 */
Vector classifier_all_docs(Index inverted, Index forward, Vector docs, int k) {
    printf("info: sorting all %d documents\n", vector_size(docs));
    Vector estimated_class = vector_new();
    Index_Map doc;
    char classname[2028];
    char *path;

    printf("progress:\n");
    vector_for(doc, docs) {
        if ((__i % 100) == 0) {
            printf("%d/%d\n", __i, vector_size(docs));
        }
        const char *class = index_classifier(inverted, forward, doc, k);
        sprintf(classname, "%s", class);
        vector_push(estimated_class, new_string(classname));
    }

    return estimated_class;
}

// matrix
/*
 * Matriz de confusao:
 * 1 - Gera um vetor de classes nao repetidas. Cada classe nesse vetor
 * representará um indice na matriz 2 - Configura a matriz: Preenche ela com 0.
 * 3 - Preenche a matriz: a classe no vetor de classes preditas representa a
 * 'linha' da matriz, e a a classe no vetor de classes estimadas representa a
 * 'coluna' da matriz. Logo para cada documento, será adicionado +1 à
 * matriz[linha][coluna]
 * 4 - Percentual de acerto : Este é calculado a seguinte fórmula - (palavras
 * classificadas corretamente * 100)/ total de palavras. O total de palavras é a
 * soma de todos os elementos da matriz e as palavras classificadas corretamente
 * são as que fazem parte da diagonal principal da matriz, onde i = j.
 * 5 - Exibe o resultado e o grava em um arquivo .txt
 */
void matrix_of_confusion(Vector predicted_doc_classes,
                         Vector estimated_doc_classes, char *dir) {
    printf("\n\n................. MATRIX ................\n\n");
    Vector vector_classes = vector_new();
    char *classname, *classname_est;
    int size, line, colun;

    // gera um vetor de classes não repetidas. Cada classe nesse vetor
    // representará um indice na matriz
    vector_for(classname, predicted_doc_classes) {
        if (classname) {
            if (!vector_search(vector_classes, str_cmp, classname)) {
                vector_push(vector_classes, new_string(classname));
            }
        }
    }

    size = vector_size(vector_classes);

    // configura a matriz
    printf("info: configuring the matrix\n");
    int matrix[size][size];
    int i, j;
    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            matrix[i][j] = 0;
        }
    }

    // preenche a matriz
    printf("info: inserting elements into the matrix\n");
    vector_for(classname, predicted_doc_classes) {
        classname_est = vector_at(estimated_doc_classes, __i);
        if (classname_est && classname) {
            line = vector_get_index(vector_classes, str_cmp, classname);
            colun = vector_get_index(vector_classes, str_cmp, classname_est);
            if (0 <= line && line < size && 0 <= colun && colun < size) {
                matrix[line][colun] += 1;
            }
        }
    }

    // percentual de acerto
    double total = 0;    // soma de todos os elementos da matriz
    double correct = 0;  // soma dos elementos da diagonal principal da matriz

    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            total += matrix[i][j];
            if (i == j) {
                correct += matrix[i][j];
            }
        }
    }
    double hit_percent = (correct * 100) / total;

    matrix_write(size, matrix, dir, hit_percent, vector_classes);
    matrix_show(size, matrix, hit_percent, vector_classes);
    vector_destroy(vector_classes, free);
}

void matrix_write(int size, int matrix[size][size], char *dir,
                  double hit_percent, Vector vector_classes) {
    FILE *file_matrix = fopen(dir, "w");

    if (!file_matrix) {
        printf("Unexpected error opening file '%s\n", dir);
        return;
    }

    char *classname;
    // imprimir percentual de acerto
    fprintf(file_matrix, "hit percentage: %.2lf%%\n", hit_percent);

    // imprimir uma linha com o nome das classes
    vector_for(classname, vector_classes) {
        fprintf(file_matrix, "%10s ", classname);
    }
    fprintf(file_matrix, "\n");

    // imprimir a matriz
    int i, j;
    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            fprintf(file_matrix, "%10d ", matrix[i][j]);
        }
        fprintf(file_matrix, "%10s\n", (char *)vector_at(vector_classes, i));
    }

    fclose(file_matrix);
}

void matrix_show(int size, int matrix[size][size], double hit_percent,
                 Vector vector_classes) {
    char *classname;
    int i, j;

    printf("\n.........................................\n");
    printf("hit percentage: %.2lf%%\n\n", hit_percent);
    vector_for(classname, vector_classes) {
        printf("%10s \t", classname);
    }

    printf("\n");
    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            printf("%10d \t", matrix[i][j]);
        }
        printf("%10s\n", (char *)vector_at(vector_classes, i));
    }
}