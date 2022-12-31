#ifndef ALGO_H
#define ALGO_H

// Vector

void vec_show_int(void *data, void *ctx);
void vec_show_string(void *data, void *ctx);
void vec_show_double(void *data, void *ctx);
void vec_show_float(void *data, void *ctx);
void vec_show_char(void *data, void *ctx);

// Callback Creator

#define call(return_type, args, body) ({ return_type _fn args body _fn; })

#endif  // ALGO_H
