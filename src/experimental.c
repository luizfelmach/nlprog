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



void setup(int argc, char *argv[], Index *inverted, Index *forward, int *k, Vector *path_docs, Vector *class_docs);
 
Vector classifier_all_docs(Index experimental, Index inverted, Index forward, int k);
void matrix_of_confusion(Vector predicted_doc_classes, Vector estimated_doc_classes, char *directory);
void show_comparison_of_document_classes(Index forward, Vector predicted_doc_classes, Vector estimated_doc_classes);
void write_confunsion_of_matrix(int size, int matrix[size][size], char *directory, double hit_percentage, Vector vector_classes);

int main(int argc, char **argv) {

    Index inverted;
    Index forward;
    Vector paths;
    Vector predicted_doc_classes;
    Vector estimated_doc_classes;
    int k;
    setup(argc, argv, &inverted, &forward, &k, &paths, &predicted_doc_classes);
    Index experimental = generate_index_experimental(inverted, forward, paths, predicted_doc_classes);
    estimated_doc_classes = classifier_all_docs(paths, inverted, forward, k);
 
    
    //show_comparison_of_document_classes(forward, predicted_doc_classes,estimated_doc_classes);
    //matrix_of_confusion(predicted_doc_classes, estimated_doc_classes, argv[2]);
    

    // to do: gravar os resultados no arquivo teste.txt
    // percentual de acerto
    // e a propria matriz de confusao
    vector_destroy(predicted_doc_classes, free);
    //vector_destroy(estimated_doc_classes, free);
    vector_destroy(paths, free);
    index_destroy(inverted);
    index_destroy(forward);
    index_destroy(experimental);
    
    return 0;
}
// fazendo ainda
Index generate_index_experimental(Index inverted, Index forward, Vector paths, Vector predicted_doc_classes){
    Index experimental = index_new();

   
    char *doc, *class, *path, word_index[2048], doc_key[2048];
    Index_Map docs;
    Index_Item di;
    insert_keys_into_forward(forward, paths, predicted_doc_classes);
    void *_;
    index_for(_, docs, inverted) {

        sprintf(word_index, "%d", __i);
        map_for(doc, di, docs) {
            class = vector_at(paths, atoi(doc));
            path = vector_at(predicted_doc_classes, atoi(doc));
            sprintf(doc_key, "%s,%s", path, class);
            index_add(forward, doc_key, word_index, index_item_freq(di));
        }
    
}
    return experimental;
}

void generate_tfidf(Index experimental, Index inverted, Index forward) {
    Index_Item di_experimental;
    Index_Map value;
    double tfidf;
    int len_docs;
    char *word_index;
    int total_docs = index_size(forward);
    void *_;

    index_for(_, value, experimental) {
        len_docs = map_size(value);

        map_for(word_index, di_experimental, value) {
            Pair p = index_at(inverted, atoi(word_index));
            if (p) {
                len_docs = map_size(pair_second(p));
            } else {
                len_docs = 0;
            }
            tfidf = index_calculate_tfidf(index_item_freq(di_experimental), len_docs, total_docs);
            index_set_tfidf(di_experimental, tfidf);
        }
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

void setup(int argc, char *argv[], Index *inverted, Index *forward, int *k, Vector *path_docs, Vector *class_docs) {
    if (argc < 4) {
        printf("usage: ./experimental <path bin> <path test> <output>.\n");
        exit(1);
    }
    // carrega o binario
    FILE *file_indexes = fopen(argv[1], "rb");
    if (!file_indexes) {
        printf("error: can not open file '%s'.\n", argv[1]);
        exit(1);
    }
    *k = atoi(argv[2]);
    *inverted = index_load(file_indexes);
    *forward = index_load(file_indexes);
    fclose(file_indexes);


    char filename_input[2048];
    sprintf(filename_input, "%s/test.txt", argv[2]); // mudar
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
        sprintf(class, "%s", temp2);
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

void show_comparison_of_document_classes(Index forward, Vector predicted_doc_classes, Vector estimated_doc_classes){
    char * path_class;
    char path[2048];
    char *estimated_classname;
    char *predicted_classname;
    void *_;
    index_for(path_class,_, forward){
        sscanf(path_class, "%[^,],%*s", path);
        estimated_classname = vector_at(estimated_doc_classes, __i);
        predicted_classname = vector_at(predicted_doc_classes, __i);
        printf("predicted class: %s \t estimated class: %s \t document: %s\n", predicted_classname , estimated_classname, path );
    }
    printf("\n\n");
}

Vector classifier_all_docs(Index experimental, Index inverted, Index forward, int k){
    Index_Map doc;
    char *doc_name;
    Vector estimated_doc_classes = vector_new();
    
    index_for(doc_name,doc, experimental){
        const char * class = index_classifier(inverted, forward, doc, k);
        char classname[2048];
        sprintf(classname,"%s",class);
        vector_push(estimated_doc_classes, new_string(classname));
    }
    
    return estimated_doc_classes;
}

int finding_class_in_vector(const void *d1, const void *d2){
    return strcmp((char*)d1,(char*)d2);
}

void matrix_of_confusion(Vector predicted_doc_classes, Vector estimated_doc_classes, char * directory){
    // vetor de classes não repetidas
    // cada posicao do vetor é uma classe e cada classe representa um indice
    Vector vector_classes = vector_new();
    char *classname;
    char *classname2;

    vector_for(classname, predicted_doc_classes){
        if(!vector_search(vector_classes,finding_class_in_vector, classname)){
            vector_push(vector_classes, new_string(classname));
        }
    }
    int size = vector_size(vector_classes);
    
    // configura a matriz
    int matrix[size][size];
    int i,j;
    for(i=0;i<size;i++){
        for(j=0;j<size;j++){
            matrix[i][j] = 0;
        }
    }

    // devem estar na mesma posicao dos dois vetores
    vector_for(classname, predicted_doc_classes){
        classname2 = vector_at(estimated_doc_classes, __i);   
        int line = vector_get_index(vector_classes, finding_class_in_vector, classname);
        int colun = vector_get_index(vector_classes, finding_class_in_vector, classname2);
        matrix[line][colun] +=1;
    }

    // imprime o resultado
    printf("                     \t");
    vector_for(classname, vector_classes){
        printf("%21s \t", classname);
    }
    printf("\n");
    for(i=0;i<size;i++){
        printf("%21s \t", (char*)vector_at(vector_classes, i));
        for(j=0;j<size;j++){
            printf("%21d \t", matrix[i][j]);
        }
        printf("\n");
    }


    // soma todos os elementos da matriz
    // soma todos os elementos diagonais da matriz
    
    // percentual de acerto
    double total = 0;
    double correct = 0;

    for(i = 0; i < size; i++){
        for(j = 0; j < size; j++){
            total += matrix[i][j];
            if(i == j){
                correct += matrix[i][j];
            }
        }
    }

    // total ---- 100
    // correct ---- x
    double hit_percentage = (correct * 100) / total;
    printf("\nacerts: %.2f%%\n", hit_percentage);
    write_confunsion_of_matrix(size, matrix, directory, hit_percentage, vector_classes);
    vector_destroy(vector_classes, free);
}


void write_confunsion_of_matrix(int size, int matrix[size][size], char *directory, double hit_percentage, Vector vector_classes){
    FILE *file_matrix = fopen(directory,"w");

    if(!file_matrix){
        printf("Unexpected error opening file '%s\n", directory);
        return;
    }

    char *classname;
    // imprimir percentual de acerto
    fprintf(file_matrix, "hit percentage: %.2lf%%\n", hit_percentage);
    
    // tamanho
    fprintf(file_matrix,"size: %d\n", size);

    // imprimir uma linha com o nome das classes
    vector_for(classname, vector_classes){
        fprintf(file_matrix, "%10s ", classname_map_get_siggle(classname));
    }
    fprintf(file_matrix,"\n");

    // imprimir a matriz
    int i,j;
    for(i = 0; i < size; i++){
        for(j = 0; j < size; j++){
            fprintf(file_matrix, "%10d ", matrix[i][j]);
        }
        fprintf(file_matrix, "\n");
    }

    fclose(file_matrix);

}

// nome do arquivo,  classe predita, 
// classe estimada


