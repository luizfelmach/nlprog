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

    char *key;
    int *value;
    map_for(key, value, map) {
        printf("%s - %d\n", key, *value);
    }

    map_destroy(map, free, free);
    return 0;
}
