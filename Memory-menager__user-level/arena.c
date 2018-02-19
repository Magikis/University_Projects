#include <math.h>
#include "queue.h"
#include "arena.h"
#include "string.h"

global Etc = {LIST_HEAD_INITIALIZER(), PTHREAD_MUTEX_INITIALIZER};
size_t pageSize;

void arenaInit() {
    pageSize = getpagesize();
}

mem_block_t *nextBlock(mem_block_t * block) {
    if (block == NULL)
        return NULL;
    return (mem_block_t*)((void*)block + abs(block->mb_size) + BLOCK_METADATA_SIZE);
}

mem_block_t *prevBlock(mem_block_t *block) {
    if (block == NULL)
        return NULL;
    size_t readTag = *(size_t*)((void*)block - 8);
    return (mem_block_t*)((void*)block - readTag);
}

int spawnChunk(size_t numPages) {
    void * addr = mmap(NULL, getpagesize() * numPages , PROT_READ | PROT_WRITE , MAP_PRIVATE | MAP_ANONYMOUS , -1, 0);
    if (addr == MAP_FAILED) {
        #ifdef DEBUG
            handle_error("Can't map memory");
        #endif
        return EXIT_FAILURE;
    }

    mem_chunk_t * chunk = (mem_chunk_t *) addr;
    LIST_INSERT_HEAD(&Etc.chunk_list, chunk, ma_node);
    chunk->size = getpagesize() * numPages - sizeof(mem_chunk_t);
    LIST_INIT(&chunk->ma_freeblks);
    // block fiiling
    // Chunk starts and ends with empty blocks, to know borders
    mem_block_t *blockAddr[4] = {&chunk->ma_first, NULL, NULL, NULL};
    int blockSize[3] = {0, chunk->size - BLOCK_METADATA_SIZE * 3, 0};

    for(int i=0; i<3; i++) {
        blockAddr[i]->mb_size = blockSize[i];
        saveTag(blockAddr[i]);
        blockAddr[i+1] = nextBlock(blockAddr[i]);
    }
    LIST_INSERT_HEAD(&chunk->ma_freeblks, blockAddr[1], mb_node);
    
    return EXIT_SUCCESS;
}

int unspawnChunk(mem_chunk_t * chunk) {
    LIST_REMOVE(chunk, ma_node);
    if ( munmap(chunk, chunk->size + sizeof(mem_chunk_t)) < 0) {
        #ifdef DEBUG
            handle_error("Can't unmap memmory");
        #endif
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

void saveTag(mem_block_t * addr) {
    void * ptr = (void *)addr + abs(addr->mb_size) + BLOCK_METADATA_SIZE - sizeof(uint64_t);
    uint64_t * tag = (uint64_t *) ptr;
    *tag = (uint64_t) (abs(addr->mb_size) + BLOCK_METADATA_SIZE); 
}

size_t ensure_valid_data(size_t dataSize, size_t alignment) {
    int powOf2 (int x) {
        while (x % 2 == 0 && x != 0) {
            x /= 2;
        }
        return x == 1 ? 1 : 0;
    };
    if (dataSize == 0) {
        return 0;
    }

    if (!powOf2(alignment) && alignment != 0) {
        return 0;
    }
    
    dataSize = dataSize < 16 ? 16 : dataSize;
    dataSize += dataSize % 8 == 0 ? 0 : 8 - (dataSize % 8); 
    return dataSize;
}

void* tryAllocBlockInAnyChunk(size_t dataSize, size_t alignment) {
    mem_chunk_t *ptr, *temp_ptr;
    LIST_FOREACH_SAFE(ptr, &Etc.chunk_list, ma_node, temp_ptr) {
       void *retPtr = tryAllocInChunk(ptr, dataSize, alignment);
       if (retPtr != NULL) {
           return retPtr;
       }
    }
    return NULL;
}

int ptrAligmnetDiff(mem_block_t * block, size_t dataSize ,size_t alignment) {
    if (!alignment)
        return 0;
    alignment = alignment < SMALLEST_BLOCK ? SMALLEST_BLOCK : alignment;
    int diff = (alignment - ((size_t)block % alignment)) - BLOCK_HEADER;
    if (diff == 0)
        return diff;
    while (diff < SMALLEST_BLOCK) {
        diff += alignment;
    }
    //Check if enough space

    return (size_t)(block->mb_size - (diff - BLOCK_HEADER) - BLOCK_HEADER) >= dataSize 
        ?   diff
        :   -1;
}

int ensureEnoughSpaceWithAlignment(mem_block_t *block, size_t dataSize, size_t alignment) {
    return (size_t)block->mb_size + BLOCK_METADATA_SIZE >= alignment + dataSize + BLOCK_METADATA_SIZE;
}

void * tryAllocInChunk(mem_chunk_t *chunk, int32_t dataSize ,size_t alignment) {
    assert(dataSize % 8 == 0);
    mem_block_t *ptr, *temp_ptr;
    LIST_FOREACH_SAFE(ptr, &chunk->ma_freeblks, mb_node, temp_ptr) {
        if (ptr->mb_size >= dataSize) {
            int diff = ptrAligmnetDiff(ptr, dataSize, alignment);
            if (diff == -1)
                continue;
            if (diff == 0 ||  ensureEnoughSpaceWithAlignment(ptr, dataSize, diff)) {
                #ifdef DEBUG
                    assert((alignment == 0 || ((size_t)ptr + diff + BLOCK_HEADER) % alignment == 0));
                #endif
                return allocBlock(ptr, dataSize, diff);
            }
        }
        
    }
    return NULL;
}

void * allocBlock(mem_block_t * block, int32_t dataSize, int diff) {
    int blockSize[3] = {0, block->mb_size + BLOCK_METADATA_SIZE, 0};
    if (diff) {
        blockSize[0] = diff;
        blockSize[1] -= blockSize[0];
    }
    int lastSize;
    if ((lastSize = blockSize[1] - (dataSize + BLOCK_METADATA_SIZE)) >=  SMALLEST_BLOCK) {
        blockSize[2] = lastSize;
        blockSize[1] = dataSize + BLOCK_METADATA_SIZE;
    }

    mem_block_t *ptr[4] = {block, block, NULL, NULL};

    for (int i=0; i<3; i++) {
        if (ptr[i] == NULL || blockSize[i] == 0) {
            ptr[i] = NULL;
            continue;
        }
        ptr[i]->mb_size = blockSize[i] - BLOCK_METADATA_SIZE;
        saveTag(ptr[i]);
        ptr[i+1] = nextBlock(ptr[i]);
    }


    if (ptr[2] != NULL) {
        LIST_INSERT_AFTER(block, ptr[2], mb_node);
    }
    if (ptr[0] == NULL) {
        LIST_REMOVE(ptr[1], mb_node);
    }

    ptr[1]->mb_size *= -1;

    return &ptr[1]->mb_data;
}

mem_chunk_t *allocationOfNewChunk(size_t dataSize, size_t alignment) {
    size_t pageesNeeded(size_t data) {
        int numOfPages = MIN_PAGES_IN_CHUNK;
        data += sizeof(mem_chunk_t) + 3 * BLOCK_METADATA_SIZE + 2 * alignment;
        while (data > numOfPages * (size_t)getpagesize()) {
            numOfPages++;
        }
        return numOfPages;
    }
    size_t numOfPages = pageesNeeded(dataSize);
    if (spawnChunk(numOfPages)) {
        return NULL;
    }   
    return LIST_FIRST(&Etc.chunk_list);
}

void *allocationInterface(size_t dataSize , size_t alignment) {
    dataSize = ensure_valid_data(dataSize, alignment);
    if (dataSize == 0) {
        return NULL;
    }
    // Big size to allocate => New chunk
    if (dataSize + 2 * alignment > 2 * (size_t)getpagesize() + (size_t) getpagesize() / 2) {
        mem_chunk_t *chunkPtr = allocationOfNewChunk(dataSize, alignment);
        if (chunkPtr == NULL) {
            return NULL;
        }
        return tryAllocInChunk(chunkPtr, dataSize, alignment);
    }

    // Try to find free block anywhere
    void *retPtr = tryAllocBlockInAnyChunk(dataSize, alignment);
    // Coudln't find free block, alloc new chunk
    if (retPtr == NULL) {
        if (spawnChunk(3)) {
            return NULL;
        }
        return tryAllocInChunk(LIST_FIRST(&Etc.chunk_list), dataSize, alignment);
    }

    return retPtr;
}

int onlyBLockInChunk(mem_block_t *block) {
    return nextBlock(block)->mb_size == 0 && prevBlock(block)->mb_size == 0;
}

mem_block_t *mergeFreeBlocks(mem_block_t *blockL, mem_block_t *blockR) {
    if (blockR < blockL) {
        mem_block_t * temp = blockR;
        blockR = blockL;
        blockL = temp;
    }
    int32_t newSize = blockL->mb_size + blockR->mb_size + BLOCK_METADATA_SIZE;
    blockL->mb_size = newSize;
    saveTag(blockL);
    return blockL;
}

mem_chunk_t *findChunkWithAddr(void *addr) {
    mem_chunk_t *ptr;
    LIST_FOREACH(ptr, &Etc.chunk_list, ma_node) {
        if ( addr > (void*)ptr && addr < (void*)ptr + ptr->size + sizeof(mem_chunk_t) ) {
            mem_block_t *block = &ptr->ma_first;
            do {
                block = nextBlock(block);
                if ((void*)block == addr) {
                    return ptr;
                }
            } while (block->mb_size != 0);
            return NULL;
        }
    }

    return NULL;
}

mem_block_t *findPrevFreeBlock(mem_block_t *block) {
    mem_block_t *ptr;
    ptr = prevBlock(block);
    while (ptr->mb_size != 0) {
        if (ptr->mb_size > 0) {
            return ptr;
        }
        ptr = prevBlock(ptr);
    }
    return NULL;
}

int isFreeBlock(mem_block_t *block) {
    if (block == NULL)
        return 0;
    return block->mb_size > 0;
}

void insertBlockAfter(mem_chunk_t *chunk, mem_block_t *prev, mem_block_t *elem) {
    if (prev == NULL) {
        LIST_INSERT_HEAD(&chunk->ma_freeblks, elem, mb_node);
    } else {
        LIST_INSERT_AFTER(prev, elem, mb_node);
    }
}

void addToFreeBlocksList(mem_chunk_t *chunk, mem_block_t *elem) {
    mem_block_t *prevFree;
    prevFree = findPrevFreeBlock(elem);
    insertBlockAfter(chunk, prevFree, elem);
}

void unleashBlock(mem_block_t *block) {
    // Finding ellement, after we can add freeblock
    mem_chunk_t *chunk;
    chunk = findChunkWithAddr((void *)block);
    if(chunk == NULL)
        return;
    
    block->mb_size = abs(block->mb_size);
    // blocks[0] -> freeBlock after we add new block
    // blocks[1] -> prevBlock; blocks[2] -> nextBlock 
    mem_block_t *blocks[3] = {NULL, NULL, NULL};
    blocks[1] = prevBlock(block);
    blocks[2] = nextBlock(block);
    

    for (int i = 2; i >= 1; i--) {
        if (isFreeBlock(blocks[i])) {
            blocks[0] = LIST_PREV(blocks[i], &chunk->ma_freeblks, mem_block, mb_node);
            LIST_REMOVE(blocks[i], mb_node);
            block = mergeFreeBlocks(blocks[i], block);
        }
    }

    if (!isFreeBlock(blocks[1]) && !isFreeBlock(blocks[2])) {
        blocks[0] = findPrevFreeBlock(block);
    }

    insertBlockAfter(chunk, blocks[0], block);

    tryUnspawnChunk(chunk, block);
}

void tryUnspawnChunk(mem_chunk_t *chunk, mem_block_t *block) {
    size_t freeSpace = 0;
    if (nextBlock(block)->mb_size == 0 && prevBlock(block)->mb_size == 0) {
        mem_chunk_t *ptrChunk;
        LIST_FOREACH(ptrChunk, &Etc.chunk_list, ma_node) {
            if (ptrChunk != chunk) {
                mem_block_t *ptr;
                LIST_FOREACH(ptr, &ptrChunk->ma_freeblks, mb_node) {
                    freeSpace += ptr->mb_size;
                    if (freeSpace >= (size_t)getpagesize() * MIN_PAGES_IN_CHUNK) {
                        unspawnChunk(chunk);
                        return;
                    }
                }
            }
        }
    }
}

// Realloc section
int ensureShiftPossible(mem_block_t *block, int size) {
    return abs(block->mb_size) + size > SMALLEST_BLOCK - BLOCK_METADATA_SIZE;
}

void shiftFreeBlockStart(mem_block_t *block, int size, mem_chunk_t *chunk) {
    int prevSize = block->mb_size;
    mem_block_t *prevFreeBlock = LIST_PREV(block, &chunk->ma_freeblks, mem_block, mb_node);
    LIST_REMOVE(block, mb_node);

    block = (mem_block_t*)(((void*)block) + size);
    block->mb_size = prevSize + (-size);
    saveTag(block);
    insertBlockAfter(chunk, prevFreeBlock, block);
}

void* rellocationInterface(void *userPtr, int userSize) {

    mem_block_t *block = userPtr - BLOCK_HEADER;
    mem_chunk_t *chunk = findChunkWithAddr((void*)block);
    if (userPtr == NULL || chunk == NULL){
        return allocationInterface(userSize, 0);
    }
    mem_block_t *blockAfter = nextBlock(block);
    userSize = ensure_valid_data(userSize, 0);
    int originalBlockSize = abs(block->mb_size);
    int changeOfSize = userSize - originalBlockSize;

    if (changeOfSize > 0) { //growing
        if(isFreeBlock(blockAfter) && (blockAfter->mb_size + BLOCK_METADATA_SIZE) >= changeOfSize) {
            int leftSpace = (blockAfter->mb_size + BLOCK_METADATA_SIZE) - changeOfSize;
            if (leftSpace >= SMALLEST_BLOCK) {
                shiftFreeBlockStart(blockAfter, changeOfSize, chunk);
            } else {
                LIST_REMOVE(blockAfter, mb_node);
                changeOfSize = blockAfter->mb_size + BLOCK_METADATA_SIZE;
            }
                
            block->mb_size -= changeOfSize;
            saveTag(block);
        } else {
            void* newPtr = allocationInterface(userSize, 0);
            memcpy(newPtr, userPtr, originalBlockSize);
            unleashBlock(block);
            return newPtr;
        }
    }

    if (changeOfSize < 0) { //schrinking
        if(isFreeBlock(blockAfter)) {
            block->mb_size += abs(changeOfSize);
            saveTag(block);
            shiftFreeBlockStart(blockAfter, changeOfSize, chunk);            
        } else {
            if (abs(changeOfSize) >= SMALLEST_BLOCK) {
                block->mb_size += abs(changeOfSize);
                saveTag(block);
                
                mem_block_t *freeBlock;
                freeBlock = nextBlock(block);
                freeBlock->mb_size = abs(changeOfSize) - BLOCK_METADATA_SIZE;
                saveTag(freeBlock);
                addToFreeBlocksList(chunk, freeBlock);
            }
        }
    }
    
    return userPtr;
}

void mdump() {
    mem_chunk_t *chunk;
    static int funcCalls = 1;
    int it = 1;
    fprintf(stderr, "mdump %02d.:\n", funcCalls++);
    LIST_FOREACH(chunk, &Etc.chunk_list, ma_node) {
        printf("%02d. START_CHUNK size:%d\n", it, chunk->size );
        mem_block_t *block = &chunk->ma_first;
        int bit = 1;
        do {
            fprintf(stderr, "   [%02d] size:%d addr:%p\n",
                bit++,
                block->mb_size,
                block);
            block = nextBlock(block);
        } while (block->mb_size != 0);
        fprintf(stderr, "%02d. END_CHUNK ---------------------------------\n", it++);
    }
}

void listTests() {
    LIST_INIT(&Etc.chunk_list);
    mem_chunk_t a,b,c;
    a.size=0; b.size=1; c.size=2;
    LIST_INSERT_HEAD(&Etc.chunk_list, &a, ma_node);
    LIST_REMOVE(&a, ma_node);
    LIST_INSERT_HEAD(&Etc.chunk_list, &b, ma_node);
    LIST_INSERT_HEAD(&Etc.chunk_list, &c, ma_node);
    mem_chunk_t * i;
    LIST_FOREACH(i, &Etc.chunk_list, ma_node) {
        printf("%d\n",i->size );
    }
    puts("");
    LIST_REMOVE(&b, ma_node);
    LIST_FOREACH(i, &Etc.chunk_list, ma_node) {
        printf("%d\n",i->size );
    }
    puts("");
    LIST_INSERT_AFTER(&a, &b, ma_node);
    LIST_FOREACH(i, &Etc.chunk_list, ma_node) {
        printf("%d\n",i->size );
    }
}