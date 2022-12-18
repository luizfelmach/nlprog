#ifndef VECTOR_H
#define VECTOR_H

#include <stdlib.h>

#define vector(type) vector_new(sizeof(type));

typedef struct _vector *Vector;

Vector vector_new(size_t size);
void *vector_at(Vector vector, size_t index);
void vector_push(Vector vector, void *data);
void vector_npush(Vector vector, void *data, size_t n);
void vector_destroy(Vector vector);

#endif  // VECTOR_H
