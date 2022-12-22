#ifndef MAP_H
#define MAP_H

#include <pair.h>

typedef void (*data_destroy)(void *data);
typedef void (*data_fn)(void *data);

typedef struct _map *Map;

Map map_new();
Pair map_get(Map map, char *key);
void map_insert(Map map, char *key, void *data);
void map_foreach(Map map, data_fn fn);
void map_destroy(Map map, data_destroy destroy_key, data_destroy destroy_value);

#endif  // MAP_H