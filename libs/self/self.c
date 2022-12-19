#include <stdlib.h>

#include "self.h"


Self self_new() {
    Self self = (Self)calloc(1, sizeof(struct _self));
    return self;
}

void self_destroy(Self self) {
    free(self);
}
