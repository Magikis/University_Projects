#include "validate.h"
#include "string.h"
#include "assert.h"

#define MAX_ALLOCATED_ADDRESS 10000

#define handle_error(msg)   \
    do                      \
    {                       \
        perror(msg);        \
        exit(EXIT_FAILURE); \
    } while (0)


 addrBlock Mallocs[MAX_ALLOCATED_ADDRESS];
 int Gindex = 0;
 uint8_t magic = 0x5A;



void compareMem(void* addr, int size) {
    for (int i=0 ; i < size; i++) {
        assert(*((uint8_t*)addr+i) == magic);
    }
}

void* randomAddrMallocs(int n) {
   n %= Gindex ? Gindex : 1;
   for (int i=n; i < Gindex; i++) {
       if (Mallocs[i].addr != NULL) {
           return Mallocs[i].addr;
       }
   }
   for (int i=0; i < Gindex; i++) {
       if (Mallocs[i].addr != NULL) {
           return Mallocs[i].addr;
       }
   }
   return NULL;
}

void initMallocs() {
    for (int i=0; i < MAX_ALLOCATED_ADDRESS; i++)
        Mallocs[i].addr = NULL;
}

void insertMallocs(void *addr, size_t size, size_t alignment) {
    int index = -1;
    for( int i=0; i < Gindex; i++) {
        if (Mallocs[i].addr == NULL) {
            index = i;
            break;
        }
    }
    if (Gindex + 1 >= MAX_ALLOCATED_ADDRESS)
        handle_error("Mallocs buffer Overflow");
    if (index < 0)
        index = Gindex++;

    Mallocs[index].addr = addr;    
    Mallocs[index].size = size;    
    Mallocs[index].alignment = alignment;
    memset(addr, magic, size);
}

void removeMallocs(void *addr) {
    for (int i=0; i < Gindex; i++) {
        if (Mallocs[i].addr == addr) {
            Mallocs[i].addr = NULL;
            return;
        }
    }
    handle_error("Addr in Mallocs not dound");
}

int find_in_mallocs(void* addr) {
    for (int i=0; i < Gindex; i++) {
        if (Mallocs[i].addr == addr)
            return i;
    }
    return -1;
}

void assert_block(mem_block_t *block,char *status) {
    if (block->mb_size < 0) {
        int i = find_in_mallocs(&block->mb_data);
        assert(i >= 0);
        status[i] = 1;
        assert(Mallocs[i].addr == &block->mb_data);
        if (Mallocs[i].alignment != 0)
            assert((size_t)(&block->mb_data) % Mallocs[i].alignment == 0);
        assert(Mallocs[i].size <= abs(block->mb_size));

        compareMem(Mallocs[i].addr, abs(Mallocs[i].size));
        //Magic numbers;

        //
    }

}

void validate_chunk(mem_chunk_t *chunk, char* status) {
    mem_block_t *firstFree = LIST_FIRST(&chunk->ma_freeblks); 
    mem_block_t *ptr = &chunk->ma_first;
    assert(ptr->mb_size == 0);
    ptr = nextBlock(ptr);
    while (ptr->mb_size != 0) {
        if (ptr->mb_size > 0) {
            assert(ptr == firstFree);
            firstFree = LIST_NEXT(firstFree, mb_node);
        }
        assert_block(ptr, status);
        ptr = nextBlock(ptr);
    }
    assert(firstFree == NULL);

}

void validate_menager() {
    char status[MAX_ALLOCATED_ADDRESS];
    for (int i=0; i<MAX_ALLOCATED_ADDRESS; i++) status[i] = 0;

    mem_chunk_t *ptr;
    LIST_FOREACH(ptr, &Etc.chunk_list, ma_node) {
        validate_chunk(ptr, status);
    }
    for (int i=0; i<Gindex; i++) {
        assert(status[i] || Mallocs[i].addr == NULL);
    }
}


