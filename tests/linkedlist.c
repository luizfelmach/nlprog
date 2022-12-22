#include <linkedlist.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *names[5] = {"John", "Jullie", "Louis", "Peter", "Max"};

void ll_show(void *data, void *ctx);
int ll_cmp(const void *p1, const void *p2);

int main() {
    Linkedlist ll = linkedlist_new();

    int i;
    for (i = 0; i < 7; i++) {
        linkedlist_add(ll, strdup(names[i % 5]));
    }

    linkedlist_foreach(ll, ll_show, NULL);
    printf("\n");

    char *exists = linkedlist_search(ll, "Peter", ll_cmp);

    if (!exists) {
        printf("Name does not exists\n");
    } else {
        printf("Name '%s' exists\n", exists);
    }

    linkedlist_destroy(ll, free);
    return 0;
}

int ll_cmp(const void *p1, const void *p2) {
    return strcmp((char *)p1, (char *)p2);
}

void ll_show(void *data, void *ctx) {
    printf("%s\n", (char *)data);
}