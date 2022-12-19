#include <stdio.h>

#include "vector.h"
#include "str.h"

int main() {
    Vector vec = vector_new(str());

    vector_destroy(vec);
    return 0;
}
