#ifndef zu_included
#define zu_included

#include <stddef.h>

/**
 * Prints to `stderr` before exiting the program with a failure status.
 */
void zu_panic(char *fmt, ...);

/**
 * VTable of procedures required to implement the `zu_allocator_t` interface.
 */
typedef struct {
  void *(*reallocate_impl)(void *data, void *ptr, size_t size);
  void *(*allocate_impl)(void *data, size_t size);
  void (*deallocate_impl)(void *data, void *ptr);
} zu_allocator_vtable_t;

/**
 * Allocator interface. Allows procedures `zu_allocate`, `zu_reallocate`, and
 * `zu_deallocate`.
 */
typedef struct {
  zu_allocator_vtable_t *vtable;
  void *data;
} zu_allocator_t;

/**
 * Allocates memory for `T` using `allocator`. Optionally takes in a modifier to
 * the size of `T`, for example `*3` to allocate an array of 3 `T` items, or
 * `+sizeof(U)` to allocate `U` after `T`.
 */
#define zu_allocate(allocator, T, ...)                                         \
  ((T *)zu_allocate_buffer((allocator), sizeof(T) __VA_ARGS__))

/**
 * Allocates memory of size `size` using `allocator`.
 */
static inline void *zu_allocate_buffer(zu_allocator_t allocator, size_t size) {
  return allocator.vtable->allocate_impl(allocator.data, size);
}

/**
 * Re-allocates pointer `ptr`, using `allocator`, to a new memory address, with
 * the size of `T`. also takes the same optional parameter as `zu_allocate`. see
 * `zu_allocate` for more information.
 */
#define zu_reallocate(allocator, ptr, T, ...)                                  \
  ((T *)zu_reallocate_buffer((allocator), (ptr), sizeof(T) __VA_ARGS__))

/**
 * Re-allocates pointer `ptr`, using `allocator`, to a new memory address, with
 * the size `size`.
 */
static inline void *zu_reallocate_buffer(zu_allocator_t allocator, void *ptr,
                                         size_t size) {
  return allocator.vtable->reallocate_impl(allocator.data, ptr, size);
}

/**
 * De-allocates pointer `ptr` using `allocator`.
 */
static inline void zu_deallocate(zu_allocator_t allocator, void *ptr) {
  allocator.vtable->deallocate_impl(allocator.data, ptr);
}

/**
 * Implementation of `zu_allocator` that directly calls `malloc`, `realloc`, and
 * `free`. Panics if `malloc` or `realloc` return `nullptr`.
 */
extern zu_allocator_t zu_heap;

/**
 * `n` kibibytes in bytes
 */
static inline size_t zu_kib(size_t n) { return n * 1024; }

/**
 * `n` mebibytes in bytes
 */
static inline size_t zu_mib(size_t n) { return zu_kib(n) * 1024; }

/**
 * `n` gibibytes in bytes
 */
static inline size_t zu_gib(size_t n) { return zu_mib(n) * 1024; }

/**
 * Allocate a new arena allocator with the parent allocator `allocator`.
 * Optionally takes the page size that the arena allocator should work with.
 */
#define zu_new_arena(allocator, ...)                                           \
  zu_new_arena_##__VA_OPT__(page_size)((allocator)__VA_OPT__(, (__VA_ARGS__)))

/**
 * Internal procedure
 */
#define zu_new_arena_(allocator) zu_new_arena_page_size((allocator), zu_kib(4))

/**
 * A page allocated by an arena allocator
 */
typedef struct zu_page {
  struct zu_page *prev;
  char buffer[];
} zu_page_t;

/**
 * An arena allocator. Works by allocating a page of `page_size`, and bump
 * allocating from it. When the page runs out of space, or more data is
 * requested than is free in the current page, a new page is allocated.
 */
typedef struct {
  zu_allocator_t page_allocator;
  zu_allocator_t allocator;
  zu_page_t *page;
  size_t page_size;
  size_t used;
} zu_arena_t;

/**
 * Internal procedure.
 */
zu_arena_t *zu_new_arena_page_size(zu_allocator_t allocator, size_t page_size);

/**
 * Internal procedure.
 */
void zu_destroy_arena(zu_arena_t *arena);

/**
 * De-allocates zu struct `o`.
 */
#define zu_destroy(o) _Generic((o), zu_arena_t *: zu_destroy_arena)((o))

#ifndef zu_force_prefix

#define panic zu_panic
typedef zu_allocator_vtable_t allocator_vtable_t;
typedef zu_allocator_t allocator_t;
#define allocate zu_allocate
#define allocate_buffer zu_allocate_buffer
#define reallocate zu_reallocate
#define reallocate_buffer zu_reallocate_buffer
#define deallocate zu_deallocate
#define heap zu_heap
#define kib zu_kib
#define mib zu_mib
#define gib zu_gib
typedef zu_arena_t arena_t;
#define new_arena zu_new_arena
#define destroy zu_destroy

#endif

#endif
