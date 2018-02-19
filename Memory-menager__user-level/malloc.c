#include "malloc.h"
#include "arena.h"
#include "string.h"

void *foo_malloc(size_t size) {
    pthread_mutex_lock(&Etc.mutex);
    
    void *newPtr = NULL;
    newPtr = allocationInterface(size, 0);
    
    pthread_mutex_unlock(&Etc.mutex);
    return newPtr;
}


void *foo_calloc(size_t count, size_t size) {
    pthread_mutex_lock(&Etc.mutex);
    void *newPtr = NULL;
    newPtr = allocationInterface(size * count, 0);
    memset(newPtr, 0, size * count);
    pthread_mutex_unlock(&Etc.mutex);    
    return newPtr;
    
}


void *foo_realloc(void *ptr, size_t size) {
    pthread_mutex_lock(&Etc.mutex);
    void *newPtr = rellocationInterface(ptr, size);

    pthread_mutex_unlock(&Etc.mutex);
    return newPtr;
}


int foo_posix_memalign(void **memptr, size_t alignment, size_t size) {
    pthread_mutex_lock(&Etc.mutex);
    *memptr = allocationInterface(size, alignment);
    pthread_mutex_unlock(&Etc.mutex);
    return *memptr == NULL ? -1 : 0;
}


void foo_free(void *ptr) {
    pthread_mutex_lock(&Etc.mutex);
    unleashBlock((mem_block_t*)(ptr - BLOCK_HEADER));
    pthread_mutex_unlock(&Etc.mutex);
}