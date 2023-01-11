#include <index.h>
#include <stdio.h>

int main() {
    Index inverted = index_new();
    Index forward = index_new();

    index_add(inverted, "gol", "0");
    index_add(inverted, "gol", "0");
    index_add(inverted, "time", "0");
    index_add(inverted, "gol", "0");

    index_add(inverted, "carro", "1");
    index_add(inverted, "gol", "1");
    index_add(inverted, "roubado", "1");

    index_add(forward, "123.txt-esp", "1");
    index_add(forward, "123.txt-esp", "1");
    index_add(forward, "123.txt-esp", "1");
    index_add(forward, "123.txt-esp", "3");

    index_add(forward, "456.txt-pol", "0");
    index_add(forward, "456.txt-pol", "1");
    index_add(forward, "456.txt-pol", "2");

    printf("~~~~ Inverted Index ~~~~\n\n");
    index_show(inverted);

    printf("~~~~ Forward Index ~~~~\n\n");
    index_show(forward);

    index_destroy(inverted);
    index_destroy(forward);
    return 0;
}