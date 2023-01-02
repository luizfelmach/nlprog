#include <map.h>
#include <pair.h>
#include <primitive.h>
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
        int *p = map_get(map, words[i]);
        if (!p) {
            map_insert(map, new_string(words[i]), new_int(0));
            p = map_get(map, words[i]);
        }
        *p += 1;
    }

    printf("Total keys: %d\n\n", map_size(map));
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
