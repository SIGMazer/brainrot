#ifndef AREANA_H
#define AREANA_H

#include "mem.h"

// Default region size 4KB or 1 page of memory
#define DEFAULT_REGION_SIZE (4 * 1024) 

typedef struct Region {
    struct Region *next;
    size_t count;
    size_t capacity;
    uintptr_t data[];
} Region;

typedef struct Arena {
    Region *start, *end;
} Arena;

Region *region_new(size_t size);
void region_free(Region *region);
void *arena_alloc(Arena *arena, size_t size_bytes);
char *arena_strdup(Arena *arena, const char *str);
void arena_reset(Arena *arena);
void arena_free(Arena *arena);
#endif // AREANA_H
