#include <stdio.h>

// Vector

void vec_show_int(void *data, void *ctx) {
    printf("%d\n", *(int *)data);
}

void vec_show_string(void *data, void *ctx) {
    printf("%s\n", (char *)data);
}

void vec_show_double(void *data, void *ctx) {
    printf("%.2lf\n", *(double *)data);
}

void vec_show_float(void *data, void *ctx) {
    printf("%.2f\n", *(float *)data);
}

void vec_show_char(void *data, void *ctx) {
    printf("%c\n", *(char *)data);
}
