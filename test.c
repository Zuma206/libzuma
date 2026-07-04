#include "zuma.h"
#include <stdio.h>

int main() {
  int *count = allocate(heap, int);
  *count = 0;
  (*count)++;
  printf("count (expected 1) = %d\n", *count);
  panic("This is a planned panic! Program should now exit with status 1\n");
}
