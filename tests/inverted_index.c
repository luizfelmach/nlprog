#include <inverted_index.h>
#include <stdio.h>

int main() {
    Inverted_Index ii = inverted_index_new();

    inverted_index_add(ii, "gol", 0);
    inverted_index_add(ii, "gol", 0);
    inverted_index_add(ii, "time", 0);
    inverted_index_add(ii, "gol", 0);

    inverted_index_add(ii, "carro", 1);
    inverted_index_add(ii, "gol", 1);
    inverted_index_add(ii, "roubado", 1);

    inverted_index_show(ii);

    inverted_index_destroy(ii);
    return 0;
}