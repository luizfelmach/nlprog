#ifndef MAP_H
#define MAP_H

#include <pair.h>

typedef void (*data_destroy)(void *data);
typedef void (*data_fn)(void *data, void *ctx);
typedef int (*data_cmp)(const void *d1, const void *d2);

typedef struct _map *Map;

Map map_new();
void *map_get(Map map, char *key);
void map_insert(Map map, char *key, void *data);
void map_sort(Map map, data_cmp cmp);
Pair map_at(Map map, int pos);
int map_size(Map map);
void map_foreach(Map map, data_fn fn, void *ctx);
void map_destroy(Map map, data_destroy destroy_key, data_destroy destroy_value);

#endif  // MAP_H