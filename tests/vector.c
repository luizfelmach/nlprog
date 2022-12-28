#include <primitive.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector.h>

void vec_show(void *data, void *ctx);
int vec_desc(const void *data1, const void *data2);

int main() {
    Vector vec = vector_new();

    int i;
    for (i = 0; i < 200; i++) {
        vector_push(vec, new_int(i));
    }

    vector_foreach(vec, vec_show, NULL);
    vector_sort(vec, vec_desc);
    vector_foreach(vec, vec_show, NULL);

    vector_destroy(vec, free);
    return 0;
}

void vec_show(void *data, void *ctx) {
    int *value = (int *)data;
    printf("%d\n", *value);
}

int vec_desc(const void *data1, const void *data2) {
    const int *i1 = data1;
    const int *i2 = data2;

    return *i2 - *i1;
}