#include <linkedlist.h>
#include <map.h>
#include <stdio.h>
#include <stdlib.h>

static int MAX = 100;

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
    return 0;
}

void *map_get(Map map, char *key, data_cmp cmp) {
    int index = fn_hash(key);

    if (map->data[index] == NULL) {
        return NULL;
    }

    return linkedlist_search(map->data[index], key, cmp);
}

void map_insert(Map map, char *key, void *data) {
    int index = fn_hash(key);

    if (map->data[index] == NULL) {
        map->data[index] = linkedlist_new();
    }

    linkedlist_add(map->data[index], data);
}

void map_destroy(Map map, data_destroy destroy) {
    int i;
    for (i = 0; i < MAX; i++) {
        if (map->data[i] != NULL) {
            linkedlist_destroy(map->data[i], destroy);
        }
    }
    free(map->data);
    free(map);
}
