#include "vector.h"

#include <stdlib.h>
#include <string.h>

#include "self.h"

struct _vector {
    void *data;
    Self self;
    size_t last, capacity;
};

Vector vector_new(Self self) {
    Vector vector = (Vector)calloc(1, sizeof(struct _vector));
    vector->capacity = 100;
    vector->self = self;
    vector->data = vector->self->alloc(vector->capacity);
    return vector;
}

void *vector_at(Vector vector, size_t index) {
    if (vector->last < index) {
        return NULL;
    }
    return vector->self->at(vector->data, index);
}

void vector_push(Vector vector, void *data) {
    if (vector->last == vector->capacity) {
        vector->capacity *= 2;
        vector->data = vector->self->realloc(vector->data, vector->capacity);
    }
    vector->self->insert(vector->data, data, vector->last);
    vector->last += 1;
}

void vector_destroy(Vector vector) {
    vector->self->free(vector->data, vector->last);
    self_destroy(vector->self);
    free(vector);
}
