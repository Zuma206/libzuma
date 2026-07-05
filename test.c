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

  panic("This is a planned panic! Program should now exit with status 1\n");
}
