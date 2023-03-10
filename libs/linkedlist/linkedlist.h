#ifndef LINKEDLIST_H
#define LINKEDLIST_H

typedef int (*data_cmp)(const void *data1, const void *data2);
typedef void (*data_destroy)(void *data);
typedef void (*data_fn)(void *data, void *ctx);

typedef struct _linkedlist *Linkedlist;

Linkedlist linkedlist_new();
void linkedlist_add(Linkedlist linkedlist, void *data);
void *linkedlist_search(Linkedlist linkedlist, void *data, data_cmp cmp);
void linkedlist_foreach(Linkedlist linkedlist, data_fn fn, void *ctx);
void linkedlist_destroy(Linkedlist linkedist, data_destroy destroy);

#endif  // LINKEDLIST_H