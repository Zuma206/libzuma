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

typedef struct zu_page zu_page_t;

/**
 * `n` kibibytes in bytes
 */
#define zu_kib(n) ((n) * 1024)

/**
 * `n` mebibytes in bytes
 */
#define zu_mib(n) (zu_kib((n)) * 1024)

/**
 * `n` gibibytes in bytes
 */
#define zu_gib(n) (zu_mib((n)) * 1024)

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

#endif

#endif
