#include <map.h>
#include <pair.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    Map map = map_new();

    char *words[10] = {
        "c++", "java", "js",      "python",   "c#",
        "c",   "rust", "haskell", "assembly", "rust",
    };

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

    Pair p = map_get(map, "python");

    if (!p) {
        printf("Language does not exists\n");
    } else {
        char *key = (char *)pair_first(p);
        int *value = (int *)pair_second(p);
        printf("%s - %d\n", key, *value);
    }

    map_destroy(map, free, free);
    return 0;
}
