#include <algo.h>
#include <index.h>
#include <map.h>
#include <math.h>
#include <maths.h>
#include <pair.h>
#include <primitive.h>
#include <stdio.h>
#include <string.h>
#include <classname_map.h>

struct _index_item {
    int freq;
    double tf_idf;
};

struct _index {
    Map data;
};

// index_item

Index_Item index_item_new(int freq, double tf_idf) {
    Index_Item ii = (Index_Item)calloc(1, sizeof(struct _index_item));
    ii->freq = freq;
    ii->tf_idf = tf_idf;
    return ii;
}

void index_item_set(Index_Item ii, int freq, double tf_idf) {
    ii->freq = freq;
    ii->tf_idf = tf_idf;
}

void index_set_freq(Index_Item ii, int freq) {
    ii->freq = freq;
}

void index_set_tfidf(Index_Item ii, double tf_idf) {
    ii->tf_idf = tf_idf;
}

int index_item_freq(Index_Item ii) {
    return ii->freq;
}

double index_item_tfidf(Index_Item ii) {
    return ii->tf_idf;
}

void index_item_show(Index_Item ii) {
    printf("freq: %d \t tf-idf: %.2lf\n", ii->freq, ii->tf_idf);
}

Index_Item index_item_load(FILE *file) {
    Index_Item ii = index_item_new(0, 0);
    fread(ii, 1, sizeof(struct _index_item), file);
    return ii;
}

void index_item_write(Index_Item ii, FILE *file) {
    fwrite(ii, 1, sizeof(struct _index_item), file);
}

// index_map

void index_map_add(Index_Map im, char *key, int value) {
    Index_Item ii = map_get(im, key);
    if (!ii) {
        map_insert(im, new_string(key), index_item_new(0, 0));
        ii = map_get(im, key);
    }
    ii->freq += value;
}

void index_map_show(Index_Map im) {
    int i;
    for (i = 0; i < map_size(im); i++) {
        Pair p = map_at(im, i);
        char *key = pair_first(p);
        Index_Item ii = pair_second(p);
        printf("\t # %s \t ", key);
        index_item_show(ii);
    }
}

Index_Map index_map_load(FILE *file) {
    Index_Map im = map_new();
    char key_item[2048];
    int len, i, size;
    fread(&size, sizeof(int), 1, file);
    for (i = 0; i < size; i++) {
        fread(&len, sizeof(int), 1, file);
        fread(&key_item, sizeof(char), len, file);
        map_insert(im, new_string(key_item), index_item_load(file));
    }
    return im;
}

void index_map_write(Index_Map im, FILE *file) {
    int len, i, size;
    size = map_size(im);
    fwrite(&size, sizeof(int), 1, file);
    for (i = 0; i < size; i++) {
        Pair p = map_at(im, i);
        char *key_item = pair_first(p);
        Index_Item ii = pair_second(p);
        len = strlen(key_item) + 1;
        fwrite(&len, sizeof(int), 1, file);
        fwrite(key_item, sizeof(char), len, file);
        index_item_write(ii, file);
    }
}

// index

Index index_new() {
    Index index = (Index)calloc(1, sizeof(struct _index));
    index->data = map_new();
    return index;
}

void index_add(Index index, char *key1, char *key2, int value) {
    Index_Map im = map_get(index->data, key1);
    if (!im) {
        map_insert(index->data, new_string(key1), map_new());
        im = map_get(index->data, key1);
    }
    index_map_add(im, key2, value);
}

void index_insert(Index index, char *key) {
    Index_Map im = map_get(index->data, key);
    if (!im) {
        map_insert(index->data, new_string(key), map_new());
    }
}

void index_sort(Index index, data_cmp cmp) {
    map_sort(index->data, cmp);
}

void index_show(Index index) {
    int i;
    for (i = 0; i < map_size(index->data); i++) {
        Pair p = map_at(index->data, i);
        char *key = pair_first(p);
        Index_Map im = pair_second(p);
        printf("# %s\n", key);
        index_map_show(im);
        printf("\n");
    }
}

int index_size(Index index) {
    return map_size(index->data);
}

double index_calculate_tfidf(int freq_p_in_d, int n_docs_p_appeared,
                             int total_docs) {
    double tfidf;
    tfidf = log((double)(1 + total_docs) / (double)(1 + n_docs_p_appeared));
    tfidf += 1;
    tfidf *= freq_p_in_d;
    return tfidf;
}

Pair index_at(Index index, int pos) {
    return map_at(index->data, pos);
}

Index_Map index_get(Index index, char *key) {
    return map_get(index->data, key);
}

Pair index_get_at(Index index, char *key, int pos) {
    Index_Map value = map_get(index->data, key);
    if (!value) {
        return NULL;
    }
    return map_at(value, pos);
}

Index_Item index_at_get(Index index, int pos, char *key) {
    Pair p = map_at(index->data, pos);
    if (!p) {
        return NULL;
    }
    Index_Map im = pair_second(p);
    return map_get(im, key);
}

Index_Item index_get_get(Index index, char *key1, char *key2) {
    Index_Map im = map_get(index->data, key1);
    if (!im) {
        return NULL;
    }
    return map_get(im, key2);
}

Index index_load(FILE *file) {
    Index index = index_new();
    int i, size;
    fread(&size, sizeof(int), 1, file);
    for (i = 0; i < size; i++) {
        int len;
        char key[2048];
        fread(&len, sizeof(int), 1, file);
        fread(&key, sizeof(char), len, file);
        map_insert(index->data, new_string(key), index_map_load(file));
    }
    return index;
}

void index_write(Index index, FILE *file) {
    int i;
    int size = index_size(index);
    fwrite(&size, sizeof(int), 1, file);
    for (i = 0; i < size; i++) {
        Pair p = index_at(index, i);
        char *key = pair_first(p);
        int len = strlen(key) + 1;
        fwrite(&len, sizeof(int), 1, file);
        fwrite(key, sizeof(char), len, file);
        Index_Map im = index_get(index, key);
        index_map_write(im, file);
    }
}

void index_destroy(Index index) {
    map_destroy(index->data, free, call(void, (void *data), {
                    map_destroy((Map)data, free, free);
                }));
    free(index);
}

// classifier

const char * index_classifier(Index inverted, Index forward, Index_Map notice_classified, int k){
    void *_;
    Vector values = vector_new();
    Vector vector_classes = vector_new();
    Index_Map im;
    int *doc_index;
    Pair document, p;
    
    index_for(_, im, forward) {
      
        double cos = index_map_cosine_n1_n2(inverted, notice_classified, im);
        if(cos){ // para valores de cossenos diferentes de zero
            p = pair_new(new_int(__i), new_double(cos));  // indice e cosseno
            vector_push(values, p);
        }
    }

    vector_sort(values, decrescent_double_sort);
    vector_for(p, values) {
        if (__i >= k) {
            break;
        }
        doc_index = pair_first(p);    // index
        document = index_at(forward, *doc_index);
        char *path_class = pair_first(document);
        char class[2048];
        sscanf(path_class, "%*[^,],%s", class);
        vector_push(vector_classes, new_string(class));
    }

    const char *class = classname_most_frequently(vector_classes);  // +frequente

  

    vector_destroy(vector_classes, free);
    vector_destroy(values, call(void, (void *data),{
        pair_destroy(data, free, free);
    }));
    return class;
}

double index_map_cosine_n1_n2(Index inverted, Index_Map notice1, Index_Map notice2) {
    Vector tf_idf_n1 = vector_new();
    Vector tf_idf_n2 = vector_new();
    Index_Item d1_n1;
    Index_Item d1_n2;
    char *index;
    double tf_idf;
    double cos = 0;
    // para todas as palavras desse documento
    map_for(index, d1_n2, notice2) {
        // no indice de palaras, recupera 'word' da posicao 'index'
        Pair p = index_at(inverted, atoi(index));
        char *word = pair_first(p);

        // procura 'word' em 'words_index'
        d1_n1 = map_get(notice1, word);
        if (d1_n1) {
            tf_idf = index_item_tfidf(d1_n1);
        } else {
            // se essa palavra nao existe em 'word_index', seu tf-idf = 0
            tf_idf = 0;
        }
        vector_push(tf_idf_n1, new_double(tf_idf));
        tf_idf = index_item_tfidf(d1_n2);
        vector_push(tf_idf_n2, new_double(tf_idf));
    }

    // se nao existe nenhuma palavra em comum
    double *i, count = 0;
    vector_for(i, tf_idf_n1) {
        if (*i != 0.0) {
            count++;
        }
    }

    if (!count) {
        vector_destroy(tf_idf_n1, free);
        vector_destroy(tf_idf_n2, free);
        return 0;
    }

    // calcula o cosseno entre os dois vetores
    cos = maths_cosv1v2(tf_idf_n1, tf_idf_n2);

    vector_destroy(tf_idf_n1, free);
    vector_destroy(tf_idf_n2, free);
    return cos;
}

// sort

int decrescent_double_sort(const void *d1, const void *d2) {
    double *v1 = pair_second(*(const Pair *)d1);
    double *v2 = pair_second(*(const Pair *)d2);
    if (*v1 - *v2 < 0) {
        return 1;
    } else if (*v1 - *v2 > 0) {
        return -1;
    }
    return 0;
}

int decrescent_int_sort(const void *d1, const void *d2) {
    int *f1 = pair_second(*(const Pair *)d1);
    int *f2 = pair_second(*(const Pair *)d2);
    return *f2 - *f1;
}

int crescent_int_sort(const void *d1, const void *d2) {
    int *f1 = pair_second(*(const Pair *)d1);
    int *f2 = pair_second(*(const Pair *)d2);
    return *f1 - *f2;
}

int decrescent_item_freq_sort(const void *d1, const void *d2) {
    Index_Item i1 = pair_second(*(const Pair *)d1);
    Index_Item i2 = pair_second(*(const Pair *)d2);

    return index_item_freq(i2) - index_item_freq(i1);
}

int alphabetic_sort(const void *d1, const void *d2) {
    const Pair *p1 = d1;
    const Pair *p2 = d2;
    return strcmp((char *)pair_first(*p1), (char *)pair_first(*p2));
}