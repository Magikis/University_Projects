#include "arena.h"
#include "minunit.h"
#include "malloc.h"
#include <unistd.h>

#define MAX_DATALIST 10
static const uint8_t magic = 0x7C;

int getBlockSize(void* ad) {
	return ((mem_block_t*)(ad - BLOCK_HEADER))->mb_size;
} 

int chunksLength();
int freeBlocksLengt(mem_chunk_t * chunkPtr);

static void *addrList[MAX_DATALIST];
static size_t const dataList[MAX_DATALIST][2] = {
	{512,8},
	{45645,0},
	{125,256},
	{7623,1024},
	{1234,8},
	{7537,4},
	{713,32},
	{75,128},
	{8,256},
	{10,2},
};



void writeInBlock(void * ptr, size_t size) {
	for (size_t i=0 ; i < size; i++) {
		((uint8_t*)ptr)[i] = magic;
	}
}

static mem_block_t * block;
static mem_chunk_t * chunk;

void clear_menager() {
	mem_chunk_t *temp_var;
	LIST_FOREACH_SAFE(chunk, &Etc.chunk_list, ma_node, temp_var) {
		unspawnChunk(chunk);
	}
	LIST_INIT(&Etc.chunk_list);
}
void empty_setup(void){}
void empty_teardown(void){}

void main_test_setup(void) {
	block = (mem_block_t *) malloc(32);
	block->mb_size = 16;
	saveTag(block);
	for(int i=0; i<2; i++)
		block->mb_data[i] = 0;
}

void main_test_teardown(void) {
	clear_menager();
}

MU_TEST(new_block_tag) {
	void * ptr = (void *) block;
	mu_check( *(int *)ptr == 16);
	ptr += 8;
	mu_check( *(uint64_t *)ptr == 0);
	ptr += 8;
	mu_check( *(uint64_t *)ptr == 0);
	ptr += 8;
	mu_check( *(uint64_t *)ptr == 32);

}

MU_TEST(spawning_chunk) {
	LIST_INIT(&Etc.chunk_list);
	mu_check(chunksLength() == 0);	
	spawnChunk(5);
	mu_check(chunksLength() == 1);
	
	chunk = LIST_FIRST(&Etc.chunk_list);
	mu_check(chunk != NULL);
	mu_check(chunk->size == (int)(5 * pageSize - sizeof(mem_chunk_t)));
	mem_block_t *blockAddr[3] = {&chunk->ma_first, NULL, NULL};
	
	for (int i=0; i<2; i++) {
		blockAddr[i+1] = nextBlock(blockAddr[i]);
	}
	mu_check(blockAddr[0]->mb_size == 0);
	mu_check(blockAddr[1]->mb_size == chunk->size - BLOCK_METADATA_SIZE * 3);
	mem_block_t *mainBlock  = LIST_FIRST(&chunk->ma_freeblks);	
	mu_check(blockAddr[1] == mainBlock);
	mu_check(blockAddr[2]->mb_size == 0);

	unspawnChunk(chunk);
	mu_check(chunksLength() == 0);

}

void all_block_setup() {
	spawnChunk(5);
	chunk = LIST_FIRST(&Etc.chunk_list);	
	tryAllocInChunk(chunk, 16, 128);
    tryAllocInChunk(chunk, 88, 0);
    tryAllocInChunk(chunk, 40, 0);
}

void all_block_teardown() {
	clear_menager();
}

MU_TEST(allocating_blocks_in_specific_chunk) {

	block = nextBlock(&chunk->ma_first);

	mu_check(block->mb_size == -56);
	block = nextBlock(block);

	mu_check(block->mb_size == -16);
	// printf("%zu\n", ((size_t)(void *)block + BLOCK_HEADER) % 128);
	mu_check((((size_t)block + BLOCK_HEADER) % 128) == 0);
	block = nextBlock(block);

	mu_check(block->mb_size == -88);
}

MU_TEST(alloc_aligmnet) {
	mu_check((size_t)tryAllocInChunk(chunk, 128, 64) % 64 == 0);
	mu_check((size_t)tryAllocInChunk(chunk, 40, 256) % 256 == 0);
	mu_check((size_t)tryAllocInChunk(chunk, 72, 16) % 16 == 0);	
}

MU_TEST(alloc_borders) {
	spawnChunk(1);
	chunk = LIST_FIRST(&Etc.chunk_list);
	block = LIST_FIRST(&chunk->ma_freeblks);
	void* freeBlockBorders[2] = {(void*)block, (void*)block+ block->mb_size + BLOCK_METADATA_SIZE};
	mem_block_t *blocks[3];
	void* dataPtr= (void*)tryAllocInChunk(chunk, 128, 512);
	blocks[1] = (mem_block_t*)(dataPtr - 8);
	blocks[0] = prevBlock(blocks[1]);
	blocks[2] = nextBlock(blocks[1]);
	
	mu_assert((void*)blocks[0] == freeBlockBorders[0], "Left border");
	mu_assert((void*)nextBlock(blocks[2]) == freeBlockBorders[1], "Right border");
	unspawnChunk(chunk);
}

MU_TEST(alloc_block_anywhere) {
	mu_check(tryAllocBlockInAnyChunk(40, 0) == NULL);
	spawnChunk(1);
	chunk = LIST_FIRST(&Etc.chunk_list);
	mu_check(tryAllocBlockInAnyChunk(40, 0) != NULL);
	unspawnChunk(chunk);
}

MU_TEST(func_ensure_valid_data) {
	mu_check(ensure_valid_data(0, 0) == 0);
	mu_check(ensure_valid_data(4, 0) == 16);
	mu_check(ensure_valid_data(16, 0) == 16);
	mu_check(ensure_valid_data(7, 0) == 16);
	mu_check(ensure_valid_data(32, 8) == 32);
	mu_check(ensure_valid_data(32, 127) == 0);
	mu_check(ensure_valid_data(7, -1) == 0);
	mu_check(ensure_valid_data(-7, 0) == 0);
}

MU_TEST(malloc_only) {
	for(size_t i=0; i < MAX_DATALIST; i++) {
		addrList[i] = foo_malloc(dataList[i][0]);
		mu_check(addrList[i] != NULL);
		writeInBlock(addrList[i], dataList[i][0]);
	}
}

MU_TEST(posix_memalign_only) {
	for(size_t i=0; i < MAX_DATALIST; i++) {
		mu_check(foo_posix_memalign(&addrList[i], dataList[i][1], dataList[i][0]) == 0);
		writeInBlock(addrList[i], dataList[i][0]);
		if (dataList[i][1])
			mu_check((size_t)addrList[i] % dataList[i][1] == 0);
	}
}

void checkExactFreeBlocks(int *tab, int size) {
	mem_block_t *ptr;
	int i=0;
	char message[100];
	LIST_FOREACH(ptr, &(LIST_FIRST(&Etc.chunk_list))->ma_freeblks, mb_node) {
		if (i == size) {
			return;
		}
		sprintf(message, "Wrong freeBlocks collection. In block nr:%d.   Expected size: %d, found: %d\n", i+1, tab[i], ptr->mb_size);
		mu_assert(ptr->mb_size == tab[i], message);
		i++;
	}
}

#define SIZE_free 10
static void *addrs[SIZE_free];
static int sizes[SIZE_free] = {24, 32, 40, 48, 56, 64, 72, 80, 88, 96};
void free_init() {
	clear_menager();
	for (int i = 0; i < SIZE_free; i++) {
		addrs[i] = foo_malloc(sizes[i]);
	}
}


MU_TEST(free_order) {
	free_init();
	int expectedValues[SIZE_free] = {32, 48, 64, 80};
	for (int i=1; i < SIZE_free; i += 2) {
		unleashBlock((mem_block_t*)(addrs[i] - BLOCK_HEADER));
	}
	checkExactFreeBlocks(expectedValues, 4);
}

MU_TEST(free_reverse) {
	free_init();
	int expectedValues[SIZE_free] = {24, 40, 56, 72, 88};
	for (int i=8; i >= 0; i -= 2) {
		unleashBlock((mem_block_t*)(addrs[i] - BLOCK_HEADER));
	}
	checkExactFreeBlocks(expectedValues, 5);
}

MU_TEST(free_left) {
	free_init();
	int expectedValues[SIZE_free] = {24 + 32 + BLOCK_METADATA_SIZE};
	int index[10] = {0, 1};
	for (int i=0; i < 2; i++) {
		unleashBlock((mem_block_t*)(addrs[index[i]] - BLOCK_HEADER));
	}
	checkExactFreeBlocks(expectedValues, 1);
}

MU_TEST(free_left_middle) {
	free_init();
	int expectedValues[SIZE_free] = {sizes[0], sizes[4] + sizes[5] + BLOCK_METADATA_SIZE};
	int index[10] = {0, 4, 5};
	for (int i=0; i < 3; i++) {
		unleashBlock((mem_block_t*)(addrs[index[i]] - BLOCK_HEADER));
	}
	checkExactFreeBlocks(expectedValues, 2);
}

MU_TEST(free_right) {
	free_init();
	int expectedValues[SIZE_free] = {sizes[0] + sizes[1] + BLOCK_METADATA_SIZE};
	int index[10] = {1, 0};
	for (int i=0; i < 2; i++) {
		unleashBlock((mem_block_t*)(addrs[index[i]] - BLOCK_HEADER));
	}
	checkExactFreeBlocks(expectedValues, 1);
}

MU_TEST(free_right_middle) {
	free_init();
	int expectedValues[SIZE_free] = {sizes[0], sizes[4] + sizes[5] + BLOCK_METADATA_SIZE};
	int index[10] = {0, 5, 4};
	for (int i=0; i < 3; i++) {
		unleashBlock((mem_block_t*)(addrs[index[i]] - BLOCK_HEADER));
	}
	checkExactFreeBlocks(expectedValues, 2);
}

MU_TEST(free_both) {
	free_init();
	int expectedValues[SIZE_free] = {sizes[0], sizes[4] + sizes[5] + sizes[6] + BLOCK_METADATA_SIZE * 2};
	int index[10] = {0, 4, 6, 5};
	for (int i=0; i < 4; i++) {
		unleashBlock((mem_block_t*)(addrs[index[i]] - BLOCK_HEADER));
	}
	checkExactFreeBlocks(expectedValues, 2);
}

MU_TEST(free_all) {
	free_init();
	int expectedValues[SIZE_free] = {pageSize * 3 - sizeof(mem_chunk_t) - 3* BLOCK_METADATA_SIZE};
	int index[10] = {3,8,0,6,9,5,1,2,7,4};
	for (int i=0; i < 10; i++) {
		unleashBlock((mem_block_t*)(addrs[index[i]] - BLOCK_HEADER));
	}
	checkExactFreeBlocks(expectedValues, 1);
}

void init_realloc_tests(void **ptrs) {
	clear_menager();
	for(int i=0; i<3; i++) {
		ptrs[i] = foo_malloc(80);
	}
}

MU_TEST(realloc_schrinking1) {
	void *ptrs[5];
	init_realloc_tests(ptrs);
	
	ptrs[0] = foo_realloc(ptrs[0], 40);
	assert(getBlockSize(ptrs[0]) == -40);
	assert(nextBlock(ptrs[0] - BLOCK_HEADER)->mb_size == 24);


	int data[1] = {24};
	checkExactFreeBlocks(data, 1);
}

MU_TEST(realloc_schrinking2) {
	void *ptrs[5];
	init_realloc_tests(ptrs);

	foo_free(ptrs[1]);
	ptrs[0] = foo_realloc(ptrs[0], 40);
	assert(getBlockSize(ptrs[0]) == -40);
	assert(nextBlock(ptrs[0] - BLOCK_HEADER)->mb_size == 120);


	int data[1] = {120};
	checkExactFreeBlocks(data, 1);
}

MU_TEST(realloc_schrinking3) {
	void *ptrs[5];
	init_realloc_tests(ptrs);

	foo_free(ptrs[1]);
	ptrs[0] = foo_realloc(ptrs[0], 72);
	assert(getBlockSize(ptrs[0]) == -72);
	assert(nextBlock(ptrs[0] - BLOCK_HEADER)->mb_size == 88);


	int data[1] = {88};
	checkExactFreeBlocks(data, 1);
}

MU_TEST(realloc_schrinking4) {
	void *ptrs[5];
	init_realloc_tests(ptrs);

	ptrs[0] = foo_realloc(ptrs[0], 80);
	assert(getBlockSize(ptrs[0]) == -80);
}

MU_TEST(realloc_growing1) {
	void *ptrs[5];
	init_realloc_tests(ptrs);

	foo_free(ptrs[1]);
	ptrs[0] = foo_realloc(ptrs[0], 250);

	assert(getBlockSize(ptrs[0]) == -256);
	int data[1] = {176};
	checkExactFreeBlocks(data, 1);
}

MU_TEST(realloc_growing2) {
	void *ptrs[5];
	init_realloc_tests(ptrs);
	foo_free(ptrs[1]);
	
	ptrs[0] = foo_realloc(ptrs[0], 88);
	assert(getBlockSize(ptrs[0]) == -88);
	assert(nextBlock(ptrs[0] - BLOCK_HEADER)->mb_size == 72);


	int data[1] = {72};
	checkExactFreeBlocks(data, 1);
}

MU_TEST(realloc_growing3) {
	void *ptrs[5];
	init_realloc_tests(ptrs);
	foo_free(ptrs[1]);
	
	ptrs[0] = foo_realloc(ptrs[0], 150);
	assert(getBlockSize(ptrs[0]) == -176);
}

MU_TEST_SUITE(no_setups) {
	MU_SUITE_CONFIGURE(&empty_setup, &empty_teardown);
	MU_RUN_TEST(alloc_block_anywhere);
	MU_RUN_TEST(func_ensure_valid_data);
	MU_RUN_TEST(realloc_growing1);
	MU_RUN_TEST(realloc_growing2);
	MU_RUN_TEST(realloc_growing3);
}

MU_TEST_SUITE(main_test) {
	MU_SUITE_CONFIGURE(&main_test_setup, &main_test_teardown);

	MU_RUN_TEST(new_block_tag);
	MU_RUN_TEST(spawning_chunk);
}

MU_TEST_SUITE(one_chunk_tests) {
	MU_SUITE_CONFIGURE(&all_block_setup, &all_block_teardown);
	MU_RUN_TEST(allocating_blocks_in_specific_chunk);
	MU_RUN_TEST(alloc_aligmnet);
	MU_RUN_TEST(alloc_borders);
}

MU_TEST_SUITE(malloc_tests) {
	MU_SUITE_CONFIGURE(&all_block_setup, &all_block_teardown);
	MU_RUN_TEST(malloc_only);
	MU_RUN_TEST(posix_memalign_only);
}

MU_TEST_SUITE(free_tests) {
	MU_SUITE_CONFIGURE(&all_block_teardown, &all_block_teardown);
	MU_RUN_TEST(free_order);
	MU_RUN_TEST(free_reverse);
	MU_RUN_TEST(free_left);
	MU_RUN_TEST(free_left_middle);
	MU_RUN_TEST(free_right);
	MU_RUN_TEST(free_right_middle);
	MU_RUN_TEST(free_both);
	MU_RUN_TEST(free_all);
	MU_RUN_TEST(realloc_schrinking1);
	MU_RUN_TEST(realloc_schrinking2);
	MU_RUN_TEST(realloc_schrinking3);
	MU_RUN_TEST(realloc_schrinking4
	);	
	
}



int main() {
	arenaInit();
	MU_RUN_SUITE(main_test);
	MU_RUN_SUITE(one_chunk_tests);
	MU_RUN_SUITE(no_setups);
	MU_RUN_SUITE(malloc_tests);
	MU_RUN_SUITE(free_tests);
	
	MU_REPORT();
	return 0;
}

int chunksLength() {
	int res =0;
	mem_chunk_t * np;
	LIST_FOREACH(np, &Etc.chunk_list, ma_node) {
		res++;
	}
	return res;
}

int freeBlocksLengt(mem_chunk_t * chunkPtr) {
	int res =0;
	mem_block_t * np;
	LIST_FOREACH(np, &chunkPtr->ma_freeblks, mb_node) {
		res++;
	}
	return res;
}