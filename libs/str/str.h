#ifndef STR_H
#define STR_H

#include "self.h"

typedef struct _str *Str;

Str str_new();
void str_set(Str s, char *v);
char *str_ptr(Str s);
int str_size(Str s);
void str_destroy(Str s);

Self str();

#endif  // STR_H
