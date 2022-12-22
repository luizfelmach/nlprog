#include <map.h>
#include <pair.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *words[10] = {
    "c++", "java", "js",      "python",   "c#",
    "c",   "rust", "haskell", "assembly", "rust",
};

void map_show(void *data, void *ctx);

int main() {
    Map map = map_new();

    int i;
    for (i = 0; i < 10; i++) {
        Pair p = map_get(map, words[i]);

        if (!p) {
            int *value = malloc(sizeof(int));
            *value = 1;
            map_insert(map, strdup(words[i]), value);
        } else {
            int *value = (int *)pair_second(p);
            *value += 1;
        }
    }

    map_foreach(map, map_show, NULL);

    map_destroy(map, free, free);
    return 0;
}

void map_show(void *data, void *ctx) {
    Pair p = (Pair)data;

    char *key = (char *)pair_first(p);
    int *value = (int *)pair_second(p);

    printf("%s - %d\n", key, *value);
}
