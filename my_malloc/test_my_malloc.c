#include "my_malloc.h"

void test_split_block() {
  int * ptr1 = ff_malloc(sizeof(int));
  char * ptr2 = ff_malloc(sizeof(char));
  int * arr1 = ff_malloc(40 * sizeof(int));
  void * ptr3 = ff_malloc(8);
  *ptr1 = 1;
  *ptr2 = 'a';
  for (size_t i = 0; i < 10; i++) {
    arr1[i] = i;
  }
  print_free_blk_list();
  ff_free(arr1);
  ff_malloc(1);
  print_free_blk_list();
  ff_malloc(1);
  print_free_blk_list();
  ff_malloc(1);
  print_free_blk_list();
  ff_malloc(1);
  print_free_blk_list();
  ff_malloc(1);
  print_free_blk_list();
}

void test_coalesce() {
  int * ptr1 = ff_malloc(0);
  char * ptr2 = ff_malloc(40);
  int * ptr3 = ff_malloc(80);
  void * ptr4 = ff_malloc(120);
  print_free_blk_list();
  ff_free(ptr3);
  print_free_blk_list();
  ff_free(ptr4);
  print_free_blk_list();
  ff_free(ptr1);
  print_free_blk_list();
  ff_free(ptr2);
  print_free_blk_list();
  long unsigned heapMem = get_data_segment_size();
  long unsigned freeSpace = get_data_segment_free_space_size();
  printf("Fragmentation: %lu\n", heapMem);
  printf("Fragmentation: %lu\n", freeSpace);
}

void test_blockReusing() {
  int NUM_ITEMS = 1000;
  int ALLOC_SZ = 128;
  int i;
  int * array[NUM_ITEMS];
  int * spacing_array[NUM_ITEMS];

  for (i = 0; i < NUM_ITEMS; i++) {
    array[i] = (int *)ff_malloc(ALLOC_SZ);
    spacing_array[i] = (int *)ff_malloc(ALLOC_SZ);
  }

  for (i = 0; i < NUM_ITEMS; i++) {
    ff_free(array[i]);
  }
  for (i = 0; i < NUM_ITEMS; i++) {
    ff_free(spacing_array[i]);
  }

  for (i = NUM_ITEMS; i >= 0; i--) {
    array[i] = (int *)ff_malloc(ALLOC_SZ);
    spacing_array[i] = (int *)ff_malloc(ALLOC_SZ);
  }
  print_free_blk_list();
}

int main(int argc, char ** argv) {
  printf("Min block size: %d\n", MIN_BLOCK_SIZE);
  printf("Header size: %d\n", HEADER_SIZE);
  // test_split_block();
  // test_coalesce();
  test_blockReusing();
  return 0;
}
