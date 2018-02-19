#ifndef FOO_MALLOC
#define FOO_MALLOC 1
#include "arena.h"

#ifdef __REMOVE__FOO
    #define  foo_malloc malloc
    #define  foo_calloc calloc
    #define  foo_realloc realloc
    #define  foo_posix_memalign posix_memalign
    #define  foo_free free
#endif

void * foo_malloc(size_t size);
void * foo_calloc(size_t count, size_t size);
void * foo_realloc(void *ptr, size_t size);
int foo_posix_memalign(void **memptr, size_t alignment, size_t size);
void foo_free(void *ptr);


#endif