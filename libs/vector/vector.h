#ifndef VECTOR_H
#define VECTOR_H

#include <stdlib.h>

#define vector_for(var, vec) \
    var = vector_at(vec, 0); \
    for (int __i = 0; __i < vector_size(vec); __i++, var = vector_at(vec, __i))

typedef void (*data_fn)(void *data, void *ctx);
typedef void (*data_destroy)(void *data);
typedef int (*data_cmp)(const void *data1, const void *data2);

typedef struct _vector *Vector;

Vector vector_new();
void *vector_at(Vector vector, int index);
void vector_push(Vector vector, void *data);
void vector_foreach(Vector vector, data_fn fn, void *ctx);
void vector_sort(Vector vector, data_cmp cmp);
int vector_size(Vector vector);
int vector_get_index(Vector vector, data_cmp cmp, void *value);
void *vector_search(Vector vector, data_cmp cmp, void *value);
void vector_destroy(Vector vector, data_destroy destroy);

#endif  // VECTOR_H
