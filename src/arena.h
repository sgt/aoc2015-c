/*
Toy arena allocator.

For examples of usage, see test.c
*/

#pragma once

#include <assert.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  size_t capacity;
  size_t offset;
  void *data;
} Arena;

Arena arena_create(const size_t initial_capacity) {
  void *data = malloc(initial_capacity);
  assert(data);
  return (Arena){.capacity = initial_capacity, .offset = 0, .data = data};
}

static inline bool _is_power_of_two(size_t x) { return (x & (x - 1)) == 0; }

size_t _align_up(size_t offset, size_t alignment) {
  assert(_is_power_of_two(alignment));
  size_t mod = offset & (alignment - 1);
  return mod == 0 ? offset : offset - mod + alignment;
}

// Allocate a chunk of memory of specified in the arena.
void *arena_alloc(Arena *arena, const size_t size, size_t alignment) {
  arena->offset = _align_up(arena->offset, alignment);
  if (arena->capacity - arena->offset < size) {
    arena->capacity *= 2;
    arena->data = realloc(arena->data, arena->capacity);
    assert(arena->data);
  }
  void *ptr = &arena->data[arena->offset];
  arena->offset += size;
  return ptr;
}

// Duplicate the string, allocating it on the arena.
char *arena_strdup(Arena *arena, const char *s) {
  size_t len = strlen(s);
  char *new_s = arena_alloc(arena, len + 1, 1);
  for (auto i = 0; i < len; ++i) {
    new_s[i] = s[i];
  }
  new_s[len] = '\0';
  return new_s;
}

// Reset the arena, making pointers to all previously allocated objects invalid.
void arena_reset(Arena *arena) { arena->offset = 0; }

// Free the memory taken by the arena.
void arena_free(Arena *arena) {
  free(arena->data);
  arena->data = NULL;
}
