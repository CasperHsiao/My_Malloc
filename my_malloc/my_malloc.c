#include "my_malloc.h"

/* Expands the process data segment by the amount specified by "size".
 * Returns the pointer to the memory block allocated.. 
 */
header * addNewBlock(size_t size) {
  header * block = sbrk(size);
  block->size = size;
  heap_mem += size;
  return block;
}

/* Splits the memory block into smaller size to accomodate just the size
 * requested. Reduces the "block->size" of the memory block by "size".
 * Returns the pointer to the memory block allocated.
 */
header * splitBlock(header * block, size_t size) {
  size_t freeSizeLeft = block->size - size;
  header * block2 = (void *)block + freeSizeLeft;
  block->size = freeSizeLeft;
  block2->size = size;
  block2->status = 'A';
  block2->next = NULL;
  block2->prev = NULL;
  return block2;
}

/* Removes a memory block from the list of free memory blocks.
 */
void removeFromFreeList(header * block) {
  header * prev = block->prev;
  header * next = block->next;
  prev->next = next;
  if (next != NULL) {
    next->prev = prev;
  }
  block->status = 'A';
  block->next = NULL;
  block->prev = NULL;
}

/* Finds the memory block in the list of free memory blocks that first fits
 * the requested size, and return the pointer to the memory block. Splits the
 * block if the block size is much greater than the requested size. Otherwise,
 * allocate the entire block and remove the block from the list of free memory
 * blocks. If no good fit of free memory blocks are found we will need to add
 * new memory blocks by expanding the process data segment.
 */
header * firstFit(size_t size) {
  header * curr = FLL.next;
  while (curr != NULL) {
    if (curr->size >= size && curr->status != 'A') {  // free and fits size
      if (curr->size >= size + MIN_BLOCK_SIZE) {      // need to split box
        //free_space -= size;
        return splitBlock(curr, size);
      }
      //free_space -= curr->size;
      removeFromFreeList(curr);
      return curr;
    }
    curr = curr->next;
  }
  return addNewBlock(size);
}

/* Finds the memory block in the list of free memory blocks that best fits
 * the requested size, and return the pointer to the memory block. Splits the
 * block if the block size is much greater than the requested size. Otherwise,
 * allocate the entire block and remove the block from the list of free memory
 * blocks. If no good fit of free memory blocks are found we will need to add
 * new memory blocks by expanding the process data segment.
 */
header * bestFit(size_t size) {
  header * curr = FLL.next;
  header * best = NULL;
  while (curr != NULL) {
    if (curr->size >= size && curr->status != 'A') {  // free and fits size
      if (best == NULL) {
        best = curr;
      }
      else if (curr->size < best->size) {  // better fit size
        best = curr;
      }
      if (best->size == size) {
        break;
      }
    }
    curr = curr->next;
  }
  if (best != NULL) {
    if (best->size >= size + MIN_BLOCK_SIZE) {  // need to split box
      //free_space -= size;
      return splitBlock(best, size);
    }
    //free_space -= best->size;
    removeFromFreeList(best);
    return best;
  }
  return addNewBlock(size);
}

/* Allocates a memory block of the requested "size", including the space for
 * metadata and return the pointer to the allocated memory block after the
 * metadata address. Allocates the memory block by using the best fit policy.
 */
void * bf_malloc(size_t size) {
  size_t mem_blk_size = HEADER_SIZE + size;
  header * block = bestFit(mem_blk_size);
  return (void *)block + HEADER_SIZE;
}

/* Allocates a memory block of the requested "size", including the space for
 * metadata and return the pointer to the allocated memory block after the
 * metadata address. Allocates the memory block by using the first fit policy.
 */
void * ff_malloc(size_t size) {
  size_t mem_blk_size = HEADER_SIZE + size;
  header * block = firstFit(mem_blk_size);
  return (void *)block + HEADER_SIZE;
}

/* Merges the two free (address adjacent) memory blocks together by increasing
 * the memory block size of the first block and removing the second memory block
 * from the list of free memory blocks.
 */
void mergeTwoBlocks(header * block1, header * block2) {
  size_t newSize = block1->size + block2->size;
  block1->size = newSize;
  removeFromFreeList(block2);
}

/* Coalesces two adjacent blocks in the list of free blocks if their addresses
 * are contiguous.
 */
void coalesceAdjacentBlocks(header * block) {
  void * blockStartAddr = (void *)block;
  void * blockEndAddr = (void *)block + block->size;

  if (block->next != NULL && blockEndAddr == (void *)block->next) {
    mergeTwoBlocks(block, block->next);
  }
  if (block->prev != NULL && block->prev->status != 'H' &&
      blockStartAddr == (void *)block->prev + block->prev->size) {
    mergeTwoBlocks(block->prev, block);
  }
}

/* Adds a memory block to the list of free memory blocks. The blocks are added
 * to the list such that they are sorted in ascending order of their address.
 */
void addToFreeList(header * block) {
  header * firstFreeBlock = FLL.next;
  if (firstFreeBlock == NULL || block < firstFreeBlock) {
    FLL.next = block;
    block->status = 'F';
    block->next = firstFreeBlock;
    block->prev = &FLL;
    if (firstFreeBlock != NULL) {
      firstFreeBlock->prev = block;
    }
  }
  else {
    header * curr = firstFreeBlock;
    while (curr->next != NULL &&
           block > curr->next) {  // make sure curr < block < curr->next
      curr = curr->next;
    }
    block->status = 'F';
    block->next = curr->next;
    block->prev = curr;
    curr->next = block;
    if (block->next != NULL) {
      block->next->prev = block;
    }
  }
}

/* Frees the memory block specified by the "ptr". Coalesces adjacent memory 
 * blocks if possible.
 */
void ff_free(void * ptr) {
  header * block = ptr - HEADER_SIZE;
  addToFreeList(block);
  coalesceAdjacentBlocks(block);
}

/* Frees the memory block specified by the "ptr". Coalesces adjacent memory 
 * blocks if possible.
 */
void bf_free(void * ptr) {
  ff_free(ptr);
}

/* Returns the entire heap memory.
 */
unsigned long get_data_segment_size() {
  return heap_mem;
}

/* Returns the size of the list of free memory blocks
 */
unsigned long get_data_segment_free_space_size() {
  // return free_space;
  header * curr = FLL.next;
  size_t freeSpace = 0;
  while (curr != NULL) {
    freeSpace += curr->size;
    curr = curr->next;
  }
  return freeSpace;
}

/* Prints the list of free memory blocks.
 */
void print_free_blk_list() {
  header * curr = &FLL;
  int i = 0;
  printf("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n");
  printf("Printing free block list forward\n");
  while (curr->next != NULL) {
    printf("----------------------------------\n");
    printf("|Block %d\n", i);
    printf("|Size: %lu\n", curr->size);
    printf("|Status: %c\n", curr->status);
    printf("|StartAddr: %p\n", curr);
    printf("|EndAddr: %p\n", (void *)curr + curr->size);
    printf("----------------------------------\n");
    curr = curr->next;
    i++;
  }
  printf("----------------------------------\n");
  printf("|Block %d\n", i);
  printf("|Size: %lu\n", curr->size);
  printf("|Status: %c\n", curr->status);
  printf("|StartAddr: %p\n", curr);
  printf("|EndAddr: %p\n", (void *)curr + curr->size);
  printf("----------------------------------\n");
  i++;
  printf("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n");
  printf("Printing free block list backwards\n");
  while (curr != NULL) {
    printf("----------------------------------\n");
    printf("|Block %d\n", i);
    printf("|Size: %lu\n", curr->size);
    printf("|Status: %c\n", curr->status);
    printf("|StartAddr: %p\n", curr);
    printf("|EndAddr: %p\n", (void *)curr + curr->size);
    printf("----------------------------------\n");
    curr = curr->prev;
    i--;
  }
  printf("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n\n\n");
}
