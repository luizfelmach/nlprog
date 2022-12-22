#include <pair.h>
#include <stdlib.h>

struct _pair {
    void *first;
    void *second;
};

Pair pair_new(void *first, void *second) {
    Pair pair = (Pair)calloc(1, sizeof(struct _pair));
    pair->first = first;
    pair->second = second;
    return pair;
}

void *pair_first(Pair pair) {
    return pair->first;
}

void *pair_second(Pair pair) {
    return pair->second;
}

void pair_destroy(Pair pair, data_destroy destroy_first,
                  data_destroy destroy_second) {
}