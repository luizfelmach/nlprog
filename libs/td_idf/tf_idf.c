#include <math.h>

double tf(int freq_p_d) {
    return (double)freq_p_d;
}

double df(int total_p_in_docs) {
    return (double)total_p_in_docs;
}

double idf(int total_docs, int total_p_in_docs) {
    return log((1 + total_docs) / (1 + df(total_p_in_docs))) + 1;
}

double tf_idf(int total_docs, int freq_p_d, int total_p_in_docs) {
    return tf(freq_p_d) * idf(total_docs, total_p_in_docs);
}