/* mem.h */

#ifndef MEM_H
#define MEM_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <assert.h>

// Maximum allocation size - helps prevent integer overflow
#define MAX_ALLOC_SIZE ((size_t)-1 >> 1)

// Alignment requirement for the platform
#define ALIGNMENT sizeof(void *)

void *handle_malloc_error(size_t size);
size_t align_size(size_t size);
void *safe_malloc(size_t size);
size_t align_size(size_t size);
void *handle_malloc_error(size_t size);
void *safe_malloc_array(size_t nmemb, size_t size);
void safe_free(void **ptr);
void *safe_memcpy(void *dest, const void *src, size_t n);
char *safe_strdup(const char *str);

// Convenience macro for type-safe allocation
#define SAFE_MALLOC(type) ((type *)safe_malloc(sizeof(type)))
#define SAFE_MALLOC_ARRAY(type, n) ((type *)safe_malloc_array((n), sizeof(type)))

// Convenience macro for safer free usage
#define SAFE_FREE(ptr) safe_free((void **)&(ptr))

#endif
