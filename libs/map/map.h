#ifndef MAP_H
#define MAP_H

typedef int (*data_cmp)(const void *data1, const void *data2);
typedef void (*data_destroy)(void *data);
typedef void (*data_fn)(void *data);

typedef struct _map *Map;

Map map_new();
void *map_get(Map map, char *key, data_cmp cmp);
void map_insert(Map map, char *key, void *data);
void map_foreach(Map map, data_fn fn);
void map_destroy(Map map, data_destroy destroy);

#endif  // MAP_H