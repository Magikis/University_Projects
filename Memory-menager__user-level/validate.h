#ifndef VALIDATE
#define VALIDATE 1
#include "arena.h"

#include "validate.h"

#define MAX_ALLOCATED_ADDRESS 10000

typedef struct addr_block {
    void *addr;
    int size;
    size_t alignment;
} addrBlock;


// static addrBlock Mallocs[MAX_ALLOCATED_ADDRESS];
// static int Gindex = 0;
void compareMem(void* addr, int size);
void* randomAddrMallocs(int n);
void initMallocs();
void insertMallocs(void *addr, size_t size, size_t alignment);
void removeMallocs(void *addr);
int find_in_mallocs(void* addr);
void assert_block(mem_block_t *block,char *status);
void validate_chunk(mem_chunk_t *chunk, char* status);
void validate_menager();

extern  addrBlock Mallocs[MAX_ALLOCATED_ADDRESS];
extern  int Gindex;
extern  uint8_t magic;

#endif