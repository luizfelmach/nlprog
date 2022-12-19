#include "self.h"

#include <stdlib.h>

Self self_new() {
    Self self = (Self)calloc(1, sizeof(struct _self));
    return self;
}

void self_destroy(Self self) {
    free(self);
}
