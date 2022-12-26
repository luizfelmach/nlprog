#ifndef VECTOR_H
#define VECTOR_H

#include <stdlib.h>

typedef void (*data_fn)(void *data, void *ctx);
typedef void (*data_destroy)(void *data);
typedef int (*data_cmp)(void *data1, void *data2);

typedef struct _vector *Vector;

Vector vector_new();
void *vector_at(Vector vector, int index);
void vector_push(Vector vector, void *data);
void vector_foreach(Vector vector, data_fn fn, void *ctx);
void *vector_search(Vector vector, data_cmp cmp, void *value);
void vector_destroy(Vector vector, data_destroy destroy);

#endif  // VECTOR_H
