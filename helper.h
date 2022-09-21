//
// Created by jvlk on 20.9.22.
//

#ifndef STACK_HELPER_H
#define STACK_HELPER_H

// Constants:
#define malloc_err 99
#define null_dereference 100

#define IE_pop_empty_stack 999

// Macross

/**
 * Generates code for malloc (if null and err output)
 */
#define make_var(name, type, size) \
type name;                                                                              \
if ((name = (type )malloc(size) ) == NULL) {                                             \
    fprintf(stderr,                                                                     \
    "Not enought memory (malloc err) in line `%d`, in file `%s`, in function: `%s`",    \
    __LINE__, __FILE__, __func__);                                                      \
    exit(malloc_err);                                                                   \
    };                                                                                  \

#endif //STACK_HELPER_H
