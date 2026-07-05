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

static void *arena_allocate(void *data, size_t size) {
  arena_t *arena = data;
  if (size > arena->page_size)
    panic("fatal allocation error: %ld bytes requested from arena with %ld "
          "page size\n",
          size, arena->page_size);
  if (arena->page == nullptr || size > arena->page_size - arena->used) {
    zu_page_t *page = allocate(arena->page_allocator, zu_page_t);
    page->prev = arena->page;
    arena->page = page;
    arena->used = 0;
  }
  void *allocation = arena->page->buffer + arena->used;
  arena->used += size;
  return allocation;
}

static void arena_deallocate(void *, void *) {}

static void *arena_reallocate(void *data, void *, size_t size) {
  return arena_allocate(data, size);
}

static allocator_vtable_t arena_vtable = {
    .deallocate_impl = arena_deallocate,
    .reallocate_impl = arena_reallocate,
    .allocate_impl = arena_allocate,
};

zu_arena_t *zu_new_arena_page_size(zu_allocator_t allocator, size_t page_size) {
  arena_t *arena = allocate(allocator, arena_t);
  arena->page_allocator = allocator;
  arena->page_size = page_size;
  arena->page = nullptr;
  arena->used = 0;
  arena->allocator = (allocator_t){
      .vtable = &arena_vtable,
      .data = arena,
  };
  return arena;
}
