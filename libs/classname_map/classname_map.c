#include <classname_map.h>
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
