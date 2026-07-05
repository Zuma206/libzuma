#include "zuma.h"
#include <stdio.h>
#include <string.h>

int main() {
  int *count = allocate(heap, int);
  *count = 0;
  (*count)++;
  printf("count (expected 1) = %d\n", *count);
  deallocate(heap, count);

  arena_t *arena = new_arena(heap);
  allocator_t allocator = arena->allocator;
  int *count_1 = allocate(allocator, int);
  char *buffer = allocate(allocator, char, *kib(4));
  *count_1 = 22;
  strcpy(buffer, "Hello, World!");

  printf("22 = %d, Hello, World! = %s\n", *count_1, buffer);

  panic("This is a planned panic! Program should now exit with status 1\n");
}
