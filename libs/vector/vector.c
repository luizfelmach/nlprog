#include <stdlib.h>
#include <string.h>
#include <vector.h>

struct _vector {
    void **data;
    int last, capacity;
};

Vector vector_new() {
    Vector vector = (Vector)calloc(1, sizeof(struct _vector));
    vector->capacity = 100;
    vector->data = (void **)calloc(vector->capacity, sizeof(void *));
    return vector;
}

void *vector_at(Vector vector, int index) {
    if (vector->last <= index) {
        return NULL;
    }
    return vector->data[index];
}

void vector_push(Vector vector, void *data) {
    if (vector->last == vector->capacity) {
        vector->capacity *= 2;
        vector->data = realloc(vector->data, vector->capacity * sizeof(void *));
    }
    vector->data[vector->last] = data;
    vector->last += 1;
}

void vector_foreach(Vector vector, data_fn fn, void *ctx) {
    int i;
    for (i = 0; i < vector->last; i++) {
        fn(vector->data[i], ctx);
    }
}

void vector_destroy(Vector vector, data_destroy destroy) {
    int i;
    for (i = 0; i < vector->last; i++) {
        destroy(vector->data[i]);
    }
    free(vector->data);
    free(vector);
}