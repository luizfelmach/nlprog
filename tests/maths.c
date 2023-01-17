#include <maths.h>
#include <primitive.h>
#include <stdio.h>

int main() {
    Vector v1, v2;
    v1 = vector_new();
    v2 = vector_new();

    // v1 and v2 orthogonal
    vector_push(v1, new_int(10));
    vector_push(v1, new_int(0));
    vector_push(v1, new_int(0));

    vector_push(v2, new_int(0));
    vector_push(v2, new_int(10));
    vector_push(v2, new_int(12));

    printf("%.2lf\n", maths_cosv1v2(v1, v2));

    return 0;
}