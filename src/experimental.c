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

void setup(int argc, char **argv, Index *inverted, Index *forward, int *k);
int main(int argc, char **argv) {
    Index inverted, forward;
    int k;
    setup(argc, argv, &inverted, &forward, &k);


    index_destroy(inverted);
    index_destroy(forward);
    return 0;
}

void setup(int argc, char **argv, Index *inverted, Index *forward, int *k) {
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



