#ifndef PAIR_H
#define PAIR_H

typedef void (*data_destroy)(void *data);

typedef struct _pair *Pair;

Pair pair_new(void *first, void *second);
void *pair_first(Pair pair);
void *pair_second(Pair pair);
void pair_destroy(Pair pair, data_destroy destroy_first,
                  data_destroy destroy_second);

#endif