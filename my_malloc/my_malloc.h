#ifndef __MY_MALLOC__
#define __MY_MALLOC__
#include <stdio.h>
#include <unistd.h>

//#define ALIGNMENT 8
//#define ALIGN(size) (((size) + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1))
#define HEADER_SIZE (sizeof(header))
#define MIN_BLOCK_SIZE (HEADER_SIZE + sizeof(char))

typedef struct header_tag {
  size_t size;
  char status;
  struct header_tag * next;
  struct header_tag * prev;
} header;

static header FLL = {0, 'H', 0, 0};
static size_t free_space = 0;
static size_t heap_mem = 0;
void * ff_malloc(size_t size);
void ff_free(void * ptr);
void * bf_malloc(size_t size);
void bf_free(void * ptr);
void print_free_blk_list();
unsigned long get_data_segment_size();
unsigned long get_data_segment_free_space_size();

#endif
