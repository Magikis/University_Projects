#ifndef ARENA
#define ARENA 1

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <assert.h>
#include <pthread.h>
#include "queue.h"

typedef struct mem_block
{
    int32_t mb_size; /* !!!Block data size!!!  mb_size > 0 => free, mb_size < 0 => allocated */
    union {
        LIST_ENTRY(mem_block) mb_node; /* node on free block list, valid if block is free */
        uint64_t mb_data[0]; /* user data pointer, valid if block is allocated */
    };
} mem_block_t;

typedef struct mem_chunk
{
    LIST_ENTRY(mem_chunk) ma_node; /* node on list of all chunks */
    LIST_HEAD(, mem_block) ma_freeblks;          /* list of all free blocks in the chunk */
    int32_t size;         /* chunk size minus sizeof(mem_chunk_t) */
    mem_block_t ma_first; /* first block in the chunk */
} mem_chunk_t;

typedef struct {
    LIST_HEAD(, mem_chunk) chunk_list; /* list of all chunks */
    pthread_mutex_t mutex;
} global;

extern global Etc;
extern size_t pageSize;

#define BLOCK_HEADER 8
#define SMALLEST_BLOCK 32
#define BLOCK_METADATA_SIZE 16
#define MIN_PAGES_IN_CHUNK 3

void arenaInit();
mem_block_t *nextBlock(mem_block_t * block);
mem_block_t *prevBlock(mem_block_t *block);
int spawnChunk(size_t numPages);
void saveTag(mem_block_t * addr);
size_t ensure_valid_data(size_t dataSize, size_t alignment);
int unspawnChunk(mem_chunk_t * chunk);
void* tryAllocBlockInAnyChunk(size_t dataSize, size_t alignment);
int ptrAligmnetDiff(mem_block_t * block, size_t dataSize, size_t alignment);
void * tryAllocInChunk(mem_chunk_t *chunk, int32_t dataSize ,size_t alignment);
void * allocBlock(mem_block_t * block, int32_t dataSize, int diff);
mem_chunk_t *allocationOfNewChunk(size_t dataSize, size_t alignment);
void *allocationInterface(size_t dataSize , size_t alignment);
int onlyBLockInChunk(mem_block_t *block);
mem_block_t *mergeFreeBlocks(mem_block_t *blockL, mem_block_t *blockR);
mem_chunk_t *findChunkWithAddr(void *addr);
mem_block_t *findPrevFreeBlock(mem_block_t *block);
void unleashBlock(mem_block_t *block);
void tryUnspawnChunk(mem_chunk_t *chunk, mem_block_t *block);
void* rellocationInterface(void *userPtr, int userSize);
void mdump();
void listTests();

#define handle_error(msg)   \
    do                      \
    {                       \
        perror(msg);        \
        exit(EXIT_FAILURE); \
    } while (0)

#endif