#include <map.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Language {
    char key[20];
    int count;
};

typedef struct Language *Language_pt;

Language_pt language_new(char *key, int count);
int language_cmp(const void *data1, const void *data2);

int main() {
    Map map = map_new();

    char *words[10] = {
        "c++", "c++", "c++", "c++", "c++", "c", "rust", "rust", "rust", "rust",
    };

    Language_pt lang;
    int i;
    for (i = 0; i < 10; i++) {
        lang = (Language_pt)map_get(map, words[i], language_cmp);

        if (!lang) {
            Language_pt new_lang = language_new(words[i], 1);
            map_insert(map, words[i], new_lang);
        } else {
            lang->count += 1;
        }
    }

    Language_pt cpp = map_get(map, "c++", language_cmp);

    if (!cpp) {
        printf("Language does not exists\n");
    } else {
        printf("%s appeared %d times\n", cpp->key, cpp->count);
    }

    Language_pt javascript = map_get(map, "javascript", language_cmp);

    if (!javascript) {
        printf("Language does not exists\n");
    } else {
        printf("%s appeared %d times\n", javascript->key, javascript->count);
    }

    map_destroy(map, free);

    return 0;
}

Language_pt language_new(char *key, int count) {
    Language_pt lang = (Language_pt)calloc(1, sizeof(struct Language));
    lang->count = count;
    strcpy(lang->key, key);
    return lang;
}

int language_cmp(const void *data1, const void *data2) {
    return strcmp(((Language_pt)data1)->key, (char *)data2);
}
