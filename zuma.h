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
  ((T *)__zu_allocate((allocator), sizeof(T) __VA_ARGS__))

/**
 * Internal procedure.
 */
static inline void *__zu_allocate(zu_allocator_t allocator, size_t size) {
  return allocator.vtable->allocate_impl(allocator.data, size);
}

/**
 * Re-allocates pointer `ptr`, using `allocator`, to a new memory address, with
 * the size of `T`. Also takes the same optional parameter as `zu_allocate`. See
 * `zu_allocate` for more information.
 */
#define zu_reallocate(allocator, ptr, T, ...)                                  \
  ((T *)__zu_reallocate((allocator), (ptr), sizeof(T) __VA_ARGS__))

/**
 * Internal procedure.
 */
static inline void *__zu_reallocate(zu_allocator_t allocator, void *ptr,
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

#ifndef zu_force_prefix

#define panic zu_panic
typedef zu_allocator_vtable_t allocator_vtable_t;
typedef zu_allocator_t allocator_t;
#define allocate zu_allocate
#define reallocate zu_reallocate
#define deallocate zu_deallocate
#define heap zu_heap

#endif

#endif
