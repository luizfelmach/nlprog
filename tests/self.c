#include "self.h"

#include <stdio.h>
#include <stdlib.h>

void *my_alloc(int n) {
    return calloc(1, sizeof(int));
}

void wrong_alloc(int a, int b) {
    return;
}

int main() {
    Self self = self_new();
    self->alloc = my_alloc;
    // self->alloc = wrong_alloc;
    self->free = NULL;
    self_destroy(self);
    return 0;
}
