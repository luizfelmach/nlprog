#ifndef MAP_H
#define MAP_H

#include <pair.h>

#define map_for(key, value, map)                       \
    key = pair_first(map_at(map, 0));                  \
    value = pair_second(map_at(map, 0));               \
    for (int __i = 0; __i < map_size(map); __i++,      \
             key = pair_first((Pair)map_at(map, __i)), \
             value = pair_second(map_at(map, __i)))

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