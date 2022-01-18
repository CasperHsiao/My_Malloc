#include "my_malloc.h"

header * addNewBlock(size_t size) {
  header * block = sbrk(size);
  block->size = size;
  heap_mem += size;
  return block;
}

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

header * firstFit(size_t size) {
  header * curr = FLL.next;
  while (curr != NULL) {
    if (curr->size >= size && curr->status != 'A') {  // free and fits size
      if (curr->size >= size + MIN_BLOCK_SIZE) {      // need to split box
        free_space -= size;
        return splitBlock(curr, size);
      }
      free_space -= curr->size;
      removeFromFreeList(curr);
      return curr;
    }
    curr = curr->next;
  }
  return addNewBlock(size);
}

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
    }
    curr = curr->next;
  }
  if (best != NULL) {
    if (best->size >= size + MIN_BLOCK_SIZE) {  // need to split box
      free_space -= size;
      return splitBlock(best, size);
    }
    free_space -= best->size;
    removeFromFreeList(best);
    return best;
  }
  return addNewBlock(size);
}

void * bf_malloc(size_t size) {
  size_t mem_blk_size = HEADER_SIZE + size;
  header * block = bestFit(mem_blk_size);
  return (void *)block + HEADER_SIZE;
}

void * ff_malloc(size_t size) {
  size_t mem_blk_size = HEADER_SIZE + size;
  header * block = firstFit(mem_blk_size);
  return (void *)block + HEADER_SIZE;
}

header * mergeTwoBlocks(header * block1, header * block2) {
  size_t newSize = block1->size + block2->size;
  block1->size = newSize;
  removeFromFreeList(block2);
  return block1;
}

/*
void coalesceAdjacentBlocks(header * block) {
  void * blockStartAddr = (void *)block;
  void * blockEndAddr = (void *)block + block->size;
  header * curr = FLL.next;
  while (curr != NULL) {
    if (blockEndAddr == (void *)curr) {  // block then curr in memeory
      curr = mergeTwoBlocks(block, curr);
      // print_free_blk_list();
    }
    if (blockStartAddr == (void *)curr + curr->size) {  // curr then block in memory
      block = mergeTwoBlocks(curr, block);
      // print_free_blk_list();
    }
    curr = curr->next;
  }
}
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

/*
void addToFreeList(header * block) {
  header * firstFreeBlock = FLL.next;
  if (firstFreeBlock != NULL) {
    firstFreeBlock->prev = block;
  }
  FLL.next = block;
  block->status = 'F';
  block->next = firstFreeBlock;
  block->prev = &FLL;
}
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

void ff_free(void * ptr) {
  header * block = ptr - HEADER_SIZE;
  addToFreeList(block);
  coalesceAdjacentBlocks(block);
  free_space += block->size;
}

void bf_free(void * ptr) {
  ff_free(ptr);
}

unsigned long get_data_segment_size() {
  return heap_mem;
}

unsigned long get_data_segment_free_space_size() {
  // return free_space;
  header * curr = FLL.next;
  size_t freeSpace = 0;
  while (curr != NULL) {
    freeSpace += curr->size;
    curr = curr->next;
  }
  printf("Another free space: %lu\n", free_space);
  return freeSpace;
}

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
