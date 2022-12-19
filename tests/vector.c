#include <stdio.h>
#include <str.h>
#include <vector.h>

int main() {
    Vector vec = vector_new(str());

    Str lang1 = str_new("c++");
    Str lang2 = str_new("rust");
    Str lang3 = str_new("c");
    Str lang4 = str_new("python");

    vector_push(vec, lang1);
    vector_push(vec, lang2);
    vector_push(vec, lang3);
    vector_push(vec, lang4);

    Str rec1 = (Str)vector_at(vec, 0);
    Str rec2 = (Str)vector_at(vec, 1);
    Str rec3 = (Str)vector_at(vec, 2);
    Str rec4 = (Str)vector_at(vec, 3);

    str_set(rec1, "html is not a lang");

    printf("%s\n", str_ptr(rec1));
    printf("%s\n", str_ptr(rec2));
    printf("%s\n", str_ptr(rec3));
    printf("%s\n", str_ptr(rec4));

    vector_destroy(vec);
    return 0;
}
