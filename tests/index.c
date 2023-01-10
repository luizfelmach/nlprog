#include <index.h>
#include <stdio.h>

int main() {
    Index inverted_index = index_new();
    Index forward_index = index_new();

    index_add(inverted_index, "gol", "0");
    index_add(inverted_index, "gol", "0");
    index_add(inverted_index, "time", "0");
    index_add(inverted_index, "gol", "0");

    index_add(inverted_index, "carro", "1");
    index_add(inverted_index, "gol", "1");
    index_add(inverted_index, "roubado", "1");

    index_add(forward_index, "123.txt-esp", "1");
    index_add(forward_index, "123.txt-esp", "1");
    index_add(forward_index, "123.txt-esp", "1");
    index_add(forward_index, "123.txt-esp", "3");

    index_add(forward_index, "456.txt-pol", "0");
    index_add(forward_index, "456.txt-pol", "1");
    index_add(forward_index, "456.txt-pol", "2");

    printf("~~~~ Inverted Index ~~~~\n\n");
    index_show(inverted_index);

    printf("~~~~ Forward Index ~~~~\n\n");
    index_show(forward_index);

    index_destroy(inverted_index);
    index_destroy(forward_index);
    return 0;
}