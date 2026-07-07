#include "zuma.h"
#include <stdio.h>
#include <string.h>

int main() {
  auto count = allocate(heap, int);
  *count = 0;
  (*count)++;
  printf("count (expected 1) = %d\n", *count);
  deallocate(heap, count);

  auto arena = new_arena(heap);
  auto allocator = arena->allocator;
  auto count_1 = allocate(allocator, int);
  auto buffer = allocate(allocator, char, *kib(4));
  *count_1 = 22;
  strcpy(buffer, "Hello, World!");
  printf("22 = %d, Hello, World! = %s\n", *count_1, buffer);
  destroy(arena);

  char backing[256];
  block_t block = make_block(backing, sizeof(backing));
  allocator = to_allocator(&block);
  char *str = allocate(allocator, char, *50);
  strcpy(str, "hiii");
  printf("hiii = %s\n", str);

  tracker_t *tracker = new_tracker(heap);
  allocator = to_allocator(tracker);
  char *str_1 = allocate(allocator, char, *250);
  strcpy(str_1, "test");
  printf("test = %s\n", str_1);
  destroy(tracker);

  panic("This is a planned panic! Program should now exit with status 1\n");
}
