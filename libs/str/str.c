#include "str.h"

#include <stdlib.h>
#include <string.h>

#include "self.h"

struct _str {
    char *data;
    int last, capacity;
};

Str str_new(char *v) {
    Str s = (Str)calloc(1, sizeof(struct _str));
    s->last = strlen(v) + 1;
    s->capacity = strlen(v) + 1;
    s->data = (char *)malloc(s->last * sizeof(char));
    strcpy(s->data, v);
    return s;
}

void str_set(Str s, char *v) {
    if (!v) {
        return;
    }
    free(s->data);
    s->last = strlen(v) + 1;
    s->capacity = strlen(v) + 1;
    s->data = (char *)malloc(s->last * sizeof(char));
    strcpy(s->data, v);
}

char *str_ptr(Str s) {
    return s->data;
}

int str_size(Str s) {
    return s->last - 1;
}

void str_destroy(Str s) {
    free(s->data);
    free(s);
}

void *str_alloc(int n) {
    return calloc(n, sizeof(Str));
}

void *str_realloc(void *data, int n) {
    return realloc(data, n * sizeof(Str));
}

void str_free(void *data) {
    free(data);
}

void *str_at(void *data, int n) {
    return ((Str *)data)[n];
}

void str_insert(void *data, void *new_data, int n) {
    Str *s = (Str *)data;
    s[n] = (Str)new_data;
}

Self str() {
    Self self = self_new();

    self->alloc = str_alloc;
    self->realloc = str_realloc;
    self->free = str_free;
    self->at = str_at;
    self->insert = str_insert;

    return self;
}
