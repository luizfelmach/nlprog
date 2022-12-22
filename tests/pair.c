#include <pair.h>
#include <primitive.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char *first_name = new_string("louis");
    char *last_name = new_string("machado");

    Pair pair = pair_new(first_name, last_name);

    printf("Full name: %s %s\n", (char *)pair_first(pair),
           (char *)pair_second(pair));

    pair_destroy(pair, free, free);
    return 0;
}