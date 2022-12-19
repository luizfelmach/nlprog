#ifndef SELF_H
#define SELF_H

struct _self {
    void *(*alloc)(int n);
    void (*free)(void *data, int n);
    void *(*realloc)(void *data, int n);
    void *(*at)(void *data, int n);
    void (*insert)(void *data, void *new_data, int n);
};

typedef struct _self *Self;

Self self_new();
void self_destroy(Self self);

#endif
