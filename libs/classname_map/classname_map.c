#include <classname_map.h>
#include <primitive.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *classname_map[22][2] = {
    {"at2", "at2"},         {"bro", "Qual a Bronca"},
    {"cid", "Cidades"},     {"cit", "Ciencia e Tecnologia"},
    {"con2", "Concursos"},  {"eco", "Economia"},
    {"ept", "Esportes"},    {"esp", "Especial"},
    {"fam", "Familia"},     {"imo", "Imoveis"},
    {"inf", "Informatica"}, {"int", "Internacional"},
    {"mic", "Minha Casa"},  {"mul", "Mulher"},
    {"opi", "Opiniao"},     {"poc", "Policia"},
    {"pot", "Politica"},    {"reg", "Regional"},
    {"sro", "Sobre Rodas"}, {"tav", "Tudo a Ver"},
    {"tvt", "TV Tudo"},     {"", "(nil)"}};

const char *classname_map_get(char *key) {
    int i = 0;
    for (i = 0; i < 21; i++) {
        if (!strcmp(classname_map[i][0], key)) {
            return classname_map[i][1];
        }
    }
    return classname_map[i][1];
}

int decrescent_frequency_sort(const void *d1, const void *d2) {
    int *f1 = pair_second(*(const Pair *)d1);
    int *f2 = pair_second(*(const Pair *)d2);
    if (*f1 - *f2 < 0) {
        return 1;
    } else if (*f1 - *f2 > 0) {
        return -1;
    }
    return 0;
}

Map classname_map_frequency(Vector vector_classes, Vector vector_freq) {
    if (vector_size(vector_classes) < 1) {
        return NULL;
    }

    Map classes_map = map_new();
    Pair p;
    char key[2048];
    char *class;

    vector_for(class, vector_classes) {
        // insert class
        int *freq = map_get(classes_map, class);
        if (!freq) {
            map_insert(classes_map, new_string(class), new_int(0));
            freq = map_get(classes_map, class);
        }

        if(!vector_freq){
            *freq += 1;
        }else{
            *freq += *(int*)vector_at(vector_freq,__i);
        }
    }

    map_sort(classes_map, decrescent_frequency_sort);
    return classes_map;
}

const char *classname_map_first(Vector vector_classes, int* freq) {
    // get the first element from
    Map classes_map = classname_map_frequency(vector_classes, NULL);
    if (!classes_map) {
        return NULL;
    }

    char class[2048];
    Pair p = map_at(classes_map, 0);
    sscanf((char *)pair_first(p), "%s", class);
    *freq = *(int*)pair_second(p);

    map_destroy(classes_map, free, free);
    return classname_map_get(class);
}