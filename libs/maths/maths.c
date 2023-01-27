#include <math.h>
#include <maths.h>
#include <stdio.h>
#include <vector.h>

// linear algebra

double maths_dot(Vector v1, Vector v2) {
    if (vector_size(v1) != vector_size(v2)) {
        printf("error: maths_dot require vectors of the same size.\n");
        exit(1);
    }
    double dot = 0;
    double *a, *b;
    vector_for(a, v1) {
        b = vector_at(v2, __i);
        dot += (*a) * (*b);
    };
    return dot;
}

double maths_norm(Vector v) {
    double norm = 0;
    double *c;
    vector_for(c, v) {
        norm += pow(*c, 2);
    }
    norm = sqrt(norm);
    return norm;
}

double maths_cosv1v2(Vector v1, Vector v2) {
    return maths_dot(v1, v2) / (maths_norm(v1) * maths_norm(v2));
}