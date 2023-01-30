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


void destroy_pair_inside_vector(void *data);
Vector classifier_all_docs(Index inverted, Index forward, int k);
void setup(int argc, char **argv, Index *inverted, Index *forward, Vector* predicted_doc_classes, int *k);
void confusion_matrix(Index inverted, Index forward, Vector predicted_doc_classes, Vector estimated_doc_classes);
void show_comparison_of_document_classes(Vector predicted_doc_classes, Vector estimated_doc_classes);
int main(int argc, char **argv) {
    Index inverted, forward;
    Vector predicted_doc_classes;
    Vector estimated_doc_classes;
    int k;
    setup(argc, argv, &inverted, &forward, &predicted_doc_classes, &k);
    

    Vector estimated_doc_classes = classifier_all_docs(inverted, forward, k);
    confusion_matrix(inverted, forward, predicted_doc_classes, estimated_doc_classes);
    show_comparison_of_document_classes(predicted_doc_classes,estimated_doc_classes);

    vector_destroy(predicted_doc_classes, destroy_pair_inside_vector);
    vector_destroy(estimated_doc_classes, destroy_pair_inside_vector);
    index_destroy(inverted);
    index_destroy(forward);
    return 0;
}

void destroy_pair_inside_vector(void *data) {
    pair_destroy(data, free, free);
}

void setup(int argc, char **argv, Index *inverted, Index *forward, Vector* predicted_doc_classes,  int *k) {
    if(argc < 4){
        printf("usage: ./experimental <bin path> <test path> <number>.\n");
        exit(1);
    }
    FILE *file_indexes = fopen(argv[1], "rb");
    if (!file_indexes) {
        printf("error: can not open file '%s'.\n", argv[1]);
        exit(1);
    }

    FILE *file_test = fopen(argv[2], "r");
    if (!file_test) {
        printf("error: can not open file '%s'.\n", argv[2]);
        fclose (file_indexes);
        exit(1);
    }

    *predicted_doc_classes = vector_new();
    // carrega os nomes dos arquivos e suas respectivas classes
    while (1) {
        char doc_name[1024], class[1024];
        if (fscanf(file_test, "%s %s%*c", doc_name, class) < 1) {
            break;
        }
        sprintf(class, "%s", classname_map_get(class));
        Pair p = pair_new(new_string(doc_name), new_string(class));
        vector_push(*predicted_doc_classes, p);
    }

    *k = atoi(argv[3]);
    *inverted = index_load(file_indexes);
    *forward = index_load(file_indexes);
    fclose(file_indexes);
    fclose(file_test);
}

void show_comparison_of_document_classes(Vector predicted_doc_classes, Vector estimated_doc_classes){
    Pair p;
    vector_for(p, predicted_doc_classes){
        Pair p2 = vector_at(estimated_doc_classes, __i);
        char *classname = pair_second(p2);
        printf("Document: %s \t class: %s \t esteemed class :%s\n", (char*)pair_first(p), (char*)pair_second(p), classname );
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
        Pair p = pair_new(new_string(doc_name),new_string(classname));
        vector_push(estimated_doc_classes, p);
    }
    
    return estimated_doc_classes;
}

int finding_class_in_vector(const void *d1, const void *d2){
    return strcmp((char*)d1,(char*)d2);
}

void confusion_matrix(Index inverted, Index forward, Vector predicted_doc_classes, Vector estimated_doc_classes){
    // vetor de classes não repetidas
    // cada posicao do vetor é uma classe e cada classe representa um indice
    Vector vector_classes = vector_new();
    Pair p;
    Pair p2;
    char *classname;
    char *classname2;

    vector_for(p, predicted_doc_classes){
        char *classname = pair_second(p);
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

    vector_for(p, predicted_doc_classes){
        vector_for(p2, estimated_doc_classes){
            char *path = pair_first(p);
            char *path2 = pair_first(p2);

            if(strstr(path2,path)){ 
                classname = pair_second(p);
                classname2 = pair_second(p2);
                int line = vector_get_index(vector_classes, finding_class_in_vector, classname);
                int colun = vector_get_index(vector_classes, finding_class_in_vector, classname2);
                matrix[line][colun] +=1;
            }  
        }
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



    vector_destroy(vector_classes, free);
}


