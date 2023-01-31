#ifndef CLASSNAME_MAP_H
#define CLASSNAME_MAP_H

#include <map.h>
#include <pair.h>
#include <vector.h>
// captura o nome da classe a partir da sigla
const char *classname_map_get(char *key);
// gera um map de classes e suas frequencias ja ordenado decrescentemente
Map classname_map_generate_map_of_frequency(Vector vector_classes, Vector vector_freq);
// classe mais frequente e sua frequencia
const char *classname_most_frequently(Vector vector_classes);
#endif  // CLASSNAME_MAP_H
