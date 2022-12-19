#include <stdio.h>
#include <str.h>

int main() {
    Str content = str_new("c++ is better");

    str_set(content, "rust is a good language programmer");

    printf("Length: %d\n", str_size(content));
    printf("Content: %s\n", str_ptr(content));

    str_destroy(content);
    return 0;
}
