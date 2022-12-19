#ifndef SELF_H
#define SELF_H

struct _self {
    void *(*constructor)();
    void (*destructor)(void * data);
};

typedef struct _self *Self;

Self self_new();
void self_destroy(Self self);

#endif
