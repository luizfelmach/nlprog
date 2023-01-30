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



Vector classifier_all_docs(Index inverted, Index forward, int k);
void setup(int argc, char **argv, Index *inverted, Index *forward, int *k);
void matrix_of_confusion(Vector predicted_doc_classes, Vector estimated_doc_classes);
void show_comparison_of_document_classes(Index forward, Vector predicted_doc_classes, Vector estimated_doc_classes);
Vector save_predicted_doc_classes(Index forward, Vector predicted_doc_classes);

int main(int argc, char **argv) {
    printf("saiu");
    Index inverted, forward;
    Vector predicted_doc_classes;
    Vector estimated_doc_classes;
    int k;
    setup(argc, argv, &inverted, &forward, &k);
    predicted_doc_classes = save_predicted_doc_classes(forward, predicted_doc_classes);
    estimated_doc_classes = classifier_all_docs(inverted, forward, k);
    matrix_of_confusion(predicted_doc_classes, estimated_doc_classes);
    
    show_comparison_of_document_classes(forward, predicted_doc_classes,estimated_doc_classes);

    // to do: gravar os resultados no arquivo teste.txt
    // percentual de acerto
    // e a propria matriz de confusao

    vector_destroy(predicted_doc_classes, free);
    vector_destroy(estimated_doc_classes, free);
    index_destroy(inverted);
    index_destroy(forward);
    
    return 0;
}

void setup(int argc, char **argv, Index *inverted, Index *forward,   int *k) {
    if(argc < 4){
        printf("usage: ./experimental <bin path> <test path> <number>.\n");
        exit(1);
    }
    FILE *file_indexes = fopen(argv[1], "rb");
    if (!file_indexes) {
        printf("error: can not open file '%s'.\n", argv[1]);
        exit(1);
    }

    FILE *file_test = fopen(argv[2], "w");
    if (!file_test) {
        printf("error: can not open file '%s'.\n", argv[2]);
        fclose (file_indexes);
        exit(1);
    }

    
    *k = atoi(argv[3]);
    *inverted = index_load(file_indexes);
    *forward = index_load(file_indexes);

    fclose(file_indexes);
    fclose(file_test);
}

Vector save_predicted_doc_classes(Index forward, Vector predicted_doc_classes){
    // carrega os nomes dos arquivos e suas respectivas classes
    predicted_doc_classes = vector_new();
    void *_;
    char * path_class;
    char *class;
    index_for(path_class, _, forward){
        char classname[2048];
        sscanf(path_class, "%*[^,],%s", classname);
        sprintf(classname, "%s", classname_map_get(classname));
        vector_push(predicted_doc_classes, new_string(classname));
    }
    return predicted_doc_classes;
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
}
Vector classifier_all_docs(Index inverted, Index forward, int k){
    Index_Map doc;
    char *doc_name;
    Vector estimated_doc_classes = vector_new();
    index_for(doc_name,doc,forward){
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

void matrix_of_confusion(Vector predicted_doc_classes, Vector estimated_doc_classes){
    // vetor de classes não repetidas
    // cada posicao do vetor é uma classe e cada classe representa um indice
    Vector vector_classes = vector_new();
    Pair p;
    Pair p2;
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
    double corrects = 0;

    for(i = 0; i < size; i++){
        for(j = 0; j < size; j++){
            total += matrix[i][j];
            if(i == j){
                corrects += matrix[i][j];
            }
        }
    }
    // total ---- 100
    // corrects ---- x

    double acerts = (corrects * 100) / total;
    printf("acerts: %.2f%%\n", acerts);
    vector_destroy(vector_classes, free);
}


