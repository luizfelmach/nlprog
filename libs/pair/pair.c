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
    if (!pair) {
        return NULL;
    }
    return pair->first;
}

void *pair_second(Pair pair) {
    if (!pair) {
        return NULL;
    }
    return pair->second;
}

void pair_destroy(Pair pair, data_destroy destroy_first,
                  data_destroy destroy_second) {
    destroy_first(pair->first);
    destroy_second(pair->second);
    free(pair);
}