#include <linkedlist.h>
#include <map.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const static int MAX = 12289;  // Good prime number for hash tables

struct _map {
    Linkedlist *data;
    int capacity;
};

Map map_new() {
    Map map = (Map)calloc(1, sizeof(struct _map));
    map->capacity = MAX;
    map->data = (Linkedlist *)calloc(map->capacity, sizeof(Linkedlist));
    return map;
}

int fn_hash(char *key) {
    int sum = 0;
    int i = 0;
    while (key[i] != '\0') {
        sum += (int)key[i];
        i += 1;
    }
    return sum % MAX;
}

int map_data_cmp(const void *d1, const void *d2) {
    Pair p = (Pair)d1;
    char *key_inside = pair_first(p);
    return strcmp(key_inside, (char *)d2);
}

Pair map_get(Map map, char *key) {
    int index = fn_hash(key);

    if (map->data[index] == NULL) {
        return NULL;
    }

    return (Pair)linkedlist_search(map->data[index], key, map_data_cmp);
}

void map_insert(Map map, char *key, void *data) {
    int index = fn_hash(key);

    if (map->data[index] == NULL) {
        map->data[index] = linkedlist_new();
    }

    Pair p = pair_new(key, data);

    linkedlist_add(map->data[index], p);
}

void map_foreach(Map map, data_fn fn) {
    int i;
    for (i = 0; i < MAX; i++) {
        if (map->data[i] != NULL) {
            linkedlist_foreach(map->data[i], fn, NULL);
        }
    }
}

typedef struct {
    void (*destroy_key)(void *data);
    void (*destroy_value)(void *data);
} Destroy_Fn;

void map_destroy_pair(void *data, void *ctx) {
    Destroy_Fn *destroy_fn = (Destroy_Fn *)ctx;
    Pair p = (Pair)data;
    pair_destroy(p, destroy_fn->destroy_key, destroy_fn->destroy_value);
}

void nothing(void *data) {
}

void map_destroy(Map map, data_destroy destroy_key,
                 data_destroy destroy_value) {
    Destroy_Fn destroy_fn = {.destroy_key = destroy_key,
                             .destroy_value = destroy_value};

    int i;
    for (i = 0; i < MAX; i++) {
        if (map->data[i] != NULL) {
            linkedlist_foreach(map->data[i], map_destroy_pair, &destroy_fn);
            linkedlist_destroy(map->data[i], nothing);
        }
    }
    free(map->data);
    free(map);
}
