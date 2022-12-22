#include <primitive.h>
#include <stdlib.h>
#include <string.h>

char *new_string(char *s) {
    return strdup(s);
}

int *new_int(int a) {
    int *b = (int *)malloc(sizeof(int));
    *b = a;
    return b;
}

double *new_double(double a) {
    double *b = (double *)malloc(sizeof(double));
    *b = a;
    return b;
}

float *new_float(float a) {
    float *b = (float *)malloc(sizeof(float));
    *b = a;
    return b;
}

char *new_char(char *c) {
    char *b = (char *)malloc(sizeof(char));
    *b = *c;
    return b;
}

void do_nothing(void *a) {
}
