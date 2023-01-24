#include <linkedlist.h>
#include <map.h>
#include <primitive.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector.h>

const static int MAX = 3079;  // Good prime number for hash tables

struct _map {
    Linkedlist *data;
    Vector data_vector;
    int capacity, size;
};

Map map_new() {
    Map map = (Map)calloc(1, sizeof(struct _map));
    map->capacity = MAX;
    map->data_vector = vector_new();
    map->data = (Linkedlist *)calloc(map->capacity, sizeof(Linkedlist));
    return map;
}

int fn_hash(char *key) {
    int sum = 0;
    int i = 0;
    while (key[i] != '\0') {
        sum = (256 * sum + (int)key[i]) % MAX;
        i += 1;
    }
    return sum;
}

int map_data_cmp(const void *d1, const void *d2) {
    Pair p = (Pair)d1;
    char *key_inside = pair_first(p);
    return strcmp(key_inside, (char *)d2);
}

void *map_get(Map map, char *key) {
    int index = fn_hash(key);

    if (map->data[index] == NULL) {
        return NULL;
    }

    Pair p = (Pair)linkedlist_search(map->data[index], key, map_data_cmp);

    if (!p) {
        return NULL;
    }

    return pair_second(p);
}

void map_insert(Map map, char *key, void *data) {
    int index = fn_hash(key);

    if (map->data[index] == NULL) {
        map->data[index] = linkedlist_new();
    }

    Pair p = pair_new(key, data);

    linkedlist_add(map->data[index], p);
    vector_push(map->data_vector, p);
    map->size += 1;
}

void map_sort(Map map, data_cmp cmp) {
    vector_sort(map->data_vector, cmp);
}

void map_foreach(Map map, data_fn fn, void *ctx) {
    int i;
    for (i = 0; i < MAX; i++) {
        if (map->data[i] != NULL) {
            linkedlist_foreach(map->data[i], fn, ctx);
        }
    }
}

int map_size(Map map) {
    return map->size;
}

Pair map_at(Map map, int pos) {
    return (Pair)vector_at(map->data_vector, pos);
}

void map_destroy(Map map, data_destroy destroy_key,
                 data_destroy destroy_value) {
    void destroy_all(void *data) {
        Pair p = (Pair)data;
        pair_destroy(p, destroy_key, destroy_value);
    };

    int i;
    for (i = 0; i < MAX; i++) {
        if (map->data[i] != NULL) {
            linkedlist_destroy(map->data[i], destroy_all);
        }
    }
    vector_destroy(map->data_vector, do_nothing);
    free(map->data);
    free(map);
}
