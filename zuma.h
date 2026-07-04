#ifndef zu_included
#define zu_included

#include <stddef.h>

/**
 * Prints to `stderr` before exiting the program with a failure status.
 */
void zu_panic(char *fmt, ...);

typedef struct {
  void *(*reallocate)(void *data, void *ptr, size_t size);
  void *(*allocate)(void *data, size_t size);
  void (*deallocate)(void *data, void *ptr);
} zu_allocator_vtable;

typedef struct {
  zu_allocator_vtable *vtable;
  void *data;
} zu_allocator;

#define zu_allocate(allocator, T, ...)                                         \
  __zu_allocate((allocator), sizeof(T) __VA_ARGS__)

static inline void *__zu_allocate(zu_allocator allocator, size_t size) {
  return allocator.vtable->allocate(allocator.data, size);
}

#define zu_reallocate(allocator, ptr, T, ...)                                  \
  __zu_allocate((allocator), (ptr), sizeof(T) __VA_ARGS__)

static inline void *__zu_reallocate(zu_allocator allocator, void *ptr,
                                    size_t size) {
  return allocator.vtable->reallocate(allocator.data, ptr, size);
}

static inline void zu_deallocate(zu_allocator allocator, void *ptr) {
  allocator.vtable->deallocate(allocator.data, ptr);
}

#ifndef zu_force_prefix

#define panic zu_panic
typedef zu_allocator_vtable allocator_vtable;
typedef zu_allocator allocator;
#define allocate zu_allocate
#define reallocate zu_reallocate
#define deallocate zu_deallocate

#endif

#endif
