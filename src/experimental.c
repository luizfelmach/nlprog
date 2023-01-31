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
Vector read_document(char *path);
int ensure_exists_paths(Vector path_docs);

// classificador
Vector classifier_all_documents(Index inverted, Index forward, Vector paths,
                                int k);
Index_Map generate_news_index(Index inverted, Index forward, Vector text);


// matriz de confusao
void matrix_of_confusion(Vector predicted_doc_classes,
                         Vector estimated_doc_classes, char *directory);

void write_confunsion_of_matrix(int size, int matrix[size][size],
                                char *directory, double hit_percentage,
                                Vector vector_classes);

int main(int argc, char **argv) {
    int k;
    Index inverted, forward;
    Vector path_docs, class_docs, estimated_class;

    setup(argc, argv, &inverted, &forward, &k, &path_docs, &class_docs);

    estimated_class = classifier_all_documents(inverted, forward, path_docs, k);

    /*
    char *path, *class, *class2;
    vector_for(path, path_docs) {
        class = vector_at(class_docs, __i);
        class2 = vector_at(estimated_class, __i);
        printf("%s %s %s\n", path, class, class2);
    }
    */

    matrix_of_confusion(class_docs, estimated_class, argv[4]);

    vector_destroy(path_docs, free);
    vector_destroy(class_docs, free);
    vector_destroy(estimated_class, free);
    index_destroy(inverted);
    index_destroy(forward);
    return 0;
}

// experimental
void setup(int argc, char **argv, Index *inverted, Index *forward, int *k,
           Vector *path_docs, Vector *class_docs) {
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

    char filename_input[2048];
    sprintf(filename_input, "%s/test.txt", argv[2]);  // mudar
    printf("info: current folder is '%s'.\n", argv[2]);
    printf("info: path to file input is '%s'.\n", filename_input);

    FILE *file_input = fopen(filename_input, "r");
    if (!file_input) {
        printf("error: can not open file '%s'.\n", filename_input);
        index_destroy(*inverted);
        index_destroy(*forward);
        exit(1);
    }

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
        sprintf(path, "%s/%s", argv[2], temp1);
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

int finding_class_in_vector(const void *d1, const void *d2) {
    return strcmp((char *)d1, (char *)d2);
}

Vector read_document(char *path) {
    FILE *file_doc = fopen(path, "r");
    char word[2048];
    if (!file_doc) {
        printf("Unexpected error when trying to open the file %s\n", path);
        exit(1);
    }

    Vector doc = vector_new();

    while (!feof(file_doc)) {
        fscanf(file_doc, "%s", word);
        vector_push(doc, new_string(word));
    }

    fclose(file_doc);
    return doc;
}

// classifier
Index_Map generate_news_index(Index inverted, Index forward, Vector text) {
    Index_Map doc_indexes = map_new();
    Index_Item item;
    Index_Map word_content;
    char *word;
    double tf_idf;
    int len_docs;
    int total_docs = index_size(forward);
    // seta a frequencia
    vector_for(word, text) {
        index_map_add(doc_indexes, word, 1);
    }

    // seta  o tf-idf
    map_for(word, item, doc_indexes) {
        word_content = index_get(inverted, word);
        if (word_content) {
            len_docs = map_size(word_content);
        } else {
            len_docs = 0;
        }
        tf_idf =
            index_calculate_tfidf(index_item_freq(item), len_docs, total_docs);
        index_set_tfidf(item, tf_idf);
    }

    return doc_indexes;
}


Vector classifier_all_documents(Index inverted, Index forward, Vector paths,
                                int k) {
    Vector estimated_class = vector_new();
    char classname[2028];
    char *path;

    vector_for(path, paths) {
        Vector text = read_document(path);
        Index_Map doc_indexes = generate_news_index(inverted, forward, text);
        const char *class = index_classifier(inverted, forward, doc_indexes, k);
        sprintf(classname, "%s", class);
        vector_push(estimated_class, new_string(classname));
        map_destroy(doc_indexes, free, free);
        vector_destroy(text, free);
    }
    return estimated_class;
}

// matrix
void matrix_of_confusion(Vector predicted_doc_classes,
                         Vector estimated_doc_classes, char *directory) {
    // vetor de classes não repetidas
    // cada posicao do vetor é uma classe e cada classe representa um indice
    Vector vector_classes = vector_new();
    char *classname;
    char *classname2;

    vector_for(classname, predicted_doc_classes) {
        if (!vector_search(vector_classes, finding_class_in_vector,
                           classname)) {
            vector_push(vector_classes, new_string(classname));
        }
    }
    int size = vector_size(vector_classes);

    // configura a matriz
    int matrix[size][size];
    int i, j;
    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            matrix[i][j] = 0;
        }
    }

    // devem estar na mesma posicao dos dois vetores
    vector_for(classname, predicted_doc_classes) {
        classname2 = vector_at(estimated_doc_classes, __i);
        int line = vector_get_index(vector_classes, finding_class_in_vector,
                                    classname);
        int colun = vector_get_index(vector_classes, finding_class_in_vector,
                                     classname2);
        matrix[line][colun] += 1;
    }

    // imprime o resultado
    printf("                     \t");
    vector_for(classname, vector_classes) {
        printf("%21s \t", classname);
    }
    printf("\n");
    for (i = 0; i < size; i++) {
        printf("%21s \t", (char *)vector_at(vector_classes, i));
        for (j = 0; j < size; j++) {
            printf("%21d \t", matrix[i][j]);
        }
        printf("\n");
    }

    // soma todos os elementos da matriz
    // soma todos os elementos diagonais da matriz

    // percentual de acerto
    double total = 0;
    double correct = 0;

    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            total += matrix[i][j];
            if (i == j) {
                correct += matrix[i][j];
            }
        }
    }

    // total ---- 100
    // correct ---- x
    double hit_percentage = (correct * 100) / total;
    printf("\nacerts: %.2f%%\n", hit_percentage);
    write_confunsion_of_matrix(size, matrix, directory, hit_percentage,
                               vector_classes);
    vector_destroy(vector_classes, free);
}

void write_confunsion_of_matrix(int size, int matrix[size][size],
                                char *directory, double hit_percentage,
                                Vector vector_classes) {
    FILE *file_matrix = fopen(directory, "w");

    if (!file_matrix) {
        printf("Unexpected error opening file '%s\n", directory);
        return;
    }

    char *classname;
    // imprimir percentual de acerto
    fprintf(file_matrix, "hit percentage: %.2lf%%\n", hit_percentage);

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
        fprintf(file_matrix, "%10s\n", (char*)vector_at(vector_classes, i));
    }

    fclose(file_matrix);
}