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

void *vector_search(Vector vector, data_cmp cmp, void *value) {
    int i;
    for (i = 0; i < vector->last; i++) {
        if (!cmp(value, vector->data[i])) {
            return vector->data[i];
        }
    }
    return NULL;
}

int vector_get_index(Vector vector, data_cmp cmp, void *value) {
    int i;
    for (i = 0; i < vector->last; i++) {
        if (cmp(value, vector->data[i]) == 0) {
            return i;
        }
    }
    return -1;
}

void vector_sort(Vector vector, data_cmp cmp) {
    qsort(vector->data, vector->last, sizeof(void *), cmp);
}

int vector_size(Vector vector) {
    return vector->last;
}

void vector_destroy(Vector vector, data_destroy destroy) {
    int i;
    for (i = 0; i < vector->last; i++) {
        if(vector->data[i]!=NULL){
            destroy(vector->data[i]);
        }
    }
    free(vector->data);
    free(vector);
}