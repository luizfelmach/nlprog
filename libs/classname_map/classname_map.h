#ifndef CLASSNAME_MAP_H
#define CLASSNAME_MAP_H

#include <map.h>
#include <pair.h>
#include <vector.h>

const char *classname_map_get(char *key);
Map classname_map_frequency(Vector vector_classes);
const char *classname_map_first(Vector vector_classes, int *freq);
#endif  // CLASSNAME_MAP_H
