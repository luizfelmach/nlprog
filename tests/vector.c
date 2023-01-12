#include <algo.h>
#include <primitive.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector.h>

int vec_desc(const void *data1, const void *data2);

int main() {
    Vector vec = vector_new();

    int i, *v;
    for (i = 0; i < 10; i++) {
        vector_push(vec, new_int(i));
    }

    vector_for(v, vec) {
        printf("%d\n", *v);
    }

    vector_sort(vec, vec_desc);

    vector_for(v, vec) {
        printf("%d\n", *v);
    }

    vector_destroy(vec, free);
    return 0;
}

int vec_desc(const void *data1, const void *data2) {
    const int *i1 = data1;
    const int *i2 = data2;

    return *i2 - *i1;
}