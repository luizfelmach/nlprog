#include <linkedlist.h>
#include <stdlib.h>

typedef struct Node {
    void *data;
    struct Node *next;
} Node;

struct _linkedlist {
    Node *node;
};

Linkedlist linkedlist_new() {
    Linkedlist linkedlist = (Linkedlist)calloc(1, sizeof(struct _linkedlist));
    return linkedlist;
}

void linkedlist_add(Linkedlist linkedlist, void *data) {
    if (linkedlist->node == NULL) {
        linkedlist->node = (Node *)calloc(1, sizeof(Node));
        linkedlist->node->data = data;
        linkedlist->node->next = NULL;
        return;
    }
    Node *node = (Node *)calloc(1, sizeof(Node));
    node->data = data;
    node->next = linkedlist->node;
    linkedlist->node = node;
}

void *linkedlist_search(Linkedlist linkedlist, void *data, data_cmp cmp) {
    Node *node = linkedlist->node;
    while (node != NULL) {
        if (!cmp(node->data, data)) {
            return node->data;
        }
        node = node->next;
    }
}

void linkedlist_foreach(Linkedlist linkedlist, data_fn fn) {
    Node *node = linkedlist->node;
    while (node != NULL) {
        fn(node->data);
        node = node->next;
    }
}

void linkedlist_destroy(Linkedlist linkedlist, data_destroy destroy) {
    Node *node = linkedlist->node;
    Node *aux = NULL;
    while (node != NULL) {
        destroy(node->data);
        aux = node->next;
        free(node);
        node = aux;
    }
    free(linkedlist);
}