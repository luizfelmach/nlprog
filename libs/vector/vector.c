#include "vector.h"

#include <stdlib.h>
#include <string.h>

struct _vector {
    void **data;
    size_t last, capacity, size;
};

Vector vector_new(size_t size) {
    Vector vector = (Vector)calloc(1, sizeof(struct _vector));
    vector->size = size;
    vector->capacity = 100;
    vector->data = (void **)calloc(vector->capacity, sizeof(void *));
    return vector;
}

void *vector_at(Vector vector, size_t index) {
    return vector->data[index];
}

void vector_push(Vector vector, void *data) {
    if (vector->last == vector->capacity) {
        vector->capacity *= 2;
        vector->data = realloc(vector->data, vector->capacity * vector->size);
    }
    vector->data[vector->last] = calloc(1, vector->size);
    memcpy(vector->data[vector->last], data, vector->size);
    vector->last += 1;
}

void vector_npush(Vector vector, void *data, size_t n) {
    if (vector->last == vector->capacity) {
        vector->capacity *= 2;
        vector->data = realloc(vector->data, vector->capacity * vector->size);
    }
    vector->data[vector->last] = calloc(n, vector->size);
    memcpy(vector->data[vector->last], data, vector->size * n);
    vector->last += 1;
}

void vector_destroy(Vector vector) {
    int i;
    for (i = 0; i < vector->last; i++) {
        free(vector->data[i]);
    }
    free(vector->data);
    free(vector);
}
