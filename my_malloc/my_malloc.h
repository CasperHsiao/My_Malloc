#ifndef __MY_MALLOC__
#define __MY_MALLOC__
#include <stdio.h>
#include <unistd.h>

#define HEADER_SIZE (sizeof(header))
#define MIN_BLOCK_SIZE (HEADER_SIZE + sizeof(char))

typedef struct header_tag {
  size_t size;
  char status;
  struct header_tag * next;
  struct header_tag * prev;
} header;

static header FLL = {0, 'H', 0, 0};
static size_t heap_mem = 0;
void * ff_malloc(size_t size);
void ff_free(void * ptr);
void * bf_malloc(size_t size);
void bf_free(void * ptr);
unsigned long get_data_segment_size();
unsigned long get_data_segment_free_space_size();

header * addNewBlock(size_t size);
header * splitBlock(header * block, size_t size);
void removeFromFreeList(header * block);
header * firstFit(size_t size);
header * bestFit(size_t size);
void mergeTwoBlocks(header * block1, header * block2);
void coalesceAdjacentBlocks(header * block);
void addToFreeList(header * block);
void print_free_blk_list();

#endif
