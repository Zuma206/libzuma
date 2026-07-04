#include "zuma.h"
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

void zu_panic(char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  vfprintf(stderr, fmt, args);
  va_end(args);
  exit(1);
}

static inline void out_of_memory() {
  panic("fatal allocation error: out of memory\n");
}

static void *heap_allocate(void *, size_t size) {
  void *ptr = malloc(size);
  if (ptr == nullptr)
    out_of_memory();
  return ptr;
}
static void *heap_reallocate(void *, void *prev, size_t size) {
  void *ptr = realloc(prev, size);
  if (ptr == nullptr)
    out_of_memory();
  return ptr;
}

static void heap_deallocate(void *, void *ptr) { free(ptr); }

static allocator_vtable_t heap_vtable = {
    .deallocate_impl = heap_deallocate,
    .reallocate_impl = heap_reallocate,
    .allocate_impl = heap_allocate,
};

allocator_t zu_heap = {
    .vtable = &heap_vtable,
    .data = nullptr,
};
