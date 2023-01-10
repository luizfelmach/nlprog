#include <index.h>
#include <stdio.h>
#include <stdlib.h>
int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("error: missing parameters.\n");
        exit(1);
    }

    char file_indexes_name[2048];

    sprintf(file_indexes_name, "%s/%s", argv[1], argv[2]);

    FILE *file_indexes = fopen(file_indexes_name, "r");

    printf("info: current folder is '%s'.\n", argv[1]);
    printf("info: path to file input is '%s'.\n", file_indexes_name);

    if (!file_indexes) {
        printf("error: can not open file '%s'.\n", file_indexes_name);
        exit(1);
    }

    Index inverted_index = index_load(file_indexes);
    Index forward_index = index_load(file_indexes);

    printf("\n-------------- inverted --------------\n\n");
    index_show(inverted_index);
    printf("\n-------------- forward --------------\n\n");
    index_show(forward_index);

    fclose(file_indexes);
    index_destroy(inverted_index);
    index_destroy(forward_index);
    return 0;
}
