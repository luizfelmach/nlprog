#include "vector.h"

#include <stdio.h>

#include "str.h"

int main() {
    Vector vec = vector_new(str());

    Str name = str_new("");
    str_set(name, "Luiz Felipe Machado");

    vector_push(vec, name);
    name = str_new("");
    vector_push(vec, name);
    vector_push(vec, name);
    vector_push(vec, name);

    Str rec = (Str)vector_at(vec, 0);

    printf("%s\n", str_ptr(rec));

    str_destroy(name);
    vector_destroy(vec);
    return 0;
}
