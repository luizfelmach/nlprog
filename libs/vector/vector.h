#ifndef VECTOR_H
#define VECTOR_H

#include <stdlib.h>

#define vector(type) vector_new(sizeof(type));
/*
* O tipo vetor/vector permite armazenar  e manipular uma lista de itens do mesmo tipo seja ele qual for
*/
typedef struct _vector *Vector;
/*
* Aloca espaço de um novo vetor
*/
Vector vector_new(size_t size);
/*
* Recupera um elemento de uma posicao do vetor
*/
void *vector_at(Vector vector, size_t index);
/*
*  Adiciona um item à ultima posicao do vetor
*/
void vector_push(Vector vector, void *data);
/*
* Adiciona um item de tamanho 'n' à ultima posicao do vetor
*/
void vector_npush(Vector vector, void *data, size_t n);
/*
* Desaloca o vetor e seus itens armazenados
*/
void vector_destroy(Vector vector);

#endif  // VECTOR_H
