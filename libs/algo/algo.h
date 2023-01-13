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
#define generic_free1(function, arg1)                               \
    ({                                                              \
        void __generic_free__(void *data) { function(data, arg1); } \
        __generic_free__;                                           \
    })

#define generic_free2(function, arg1, arg2)                               \
    ({                                                                    \
        void __generic_free__(void *data) { function(data, arg1, arg2); } \
        __generic_free__;                                                 \
    })

#endif  // ALGO_H
