#ifndef VECTOR_H
#define VECTOR_H

#include "self.h"

#include <stdlib.h>

typedef struct _vector *Vector;

Vector vector_new(Self self);
void *vector_at(Vector vector, size_t index);
void vector_push(Vector vector, void *data);
void vector_destroy(Vector vector);

#endif  // VECTOR_H
