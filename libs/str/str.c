#include "str.h"

#include <stdlib.h>
#include <string.h>

#include "self.h"

struct _str {
    char *data;
    int last, capacity;
};

Str str_new() {
    Str s = (Str)calloc(1, sizeof(struct _str));
    return s;
}

void str_set(Str s, char *v) {
    if (!v) {
        return;
    }
    free(s->data);
    s->data = (char *)malloc((strlen(v) + 1) * sizeof(char));
    strcpy(s->data, v);
}

char *str_ptr(Str s) {
    return s->data;
}

int str_size(Str s) {
    return strlen(s->data);
}

void str_destroy(Str s) {
    free(s->data);
    free(s);
}

void *str_constructor() {
    return (void *)str_new();
}

void str_destructor(void *data) {
    str_destroy((Str)data);
}

void *str_alloc(int n) {
    return (Str)calloc(n, sizeof(struct _str));
}

void str_free(void *data, int n) {
    int i;
    for (i = 0; i < n; i++) {
        str_destroy(((Str)data) + i);
    }
    free(data);
}

void *str_at(void *data, int n) {
    return ((Str)data) + n;
}

void str_insert(void *data, void *new_data, int n) {
    Str str = (Str)data;
    // str[n] = *new_data;
}

Self str() {
    Self self = self_new();
    self->constructor = str_constructor;
    self->destructor = str_destructor;
    self->alloc = str_alloc;
    self->free = str_free;
    self->at = str_at;
    return self;
}
