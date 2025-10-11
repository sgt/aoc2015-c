/*
Toy arena allocator.

For examples of usage, see test.c
*/

#pragma once

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARENA_DEFAULT_ALIGNMENT 16

typedef struct {
  size_t size;
  size_t offset;
  void *data;
} Arena;

Arena arena_create(const size_t size) {
  void *data = malloc(size);
  assert(data);
  return (Arena){.size = size, .offset = 0, .data = data};
}

static inline bool _is_power_of_two(size_t x) { return (x & (x - 1)) == 0; }

size_t _align_up(size_t offset) {
  assert(_is_power_of_two(ARENA_DEFAULT_ALIGNMENT));
  const size_t alignment = ARENA_DEFAULT_ALIGNMENT;
  size_t mod = offset & (alignment - 1);
  return mod == 0 ? offset : offset - mod + alignment;
}

// Allocate a chunk of memory of specified in the arena.
void *arena_alloc(Arena *arena, const size_t size) {
  if (arena->offset + size > arena->size) {
    return NULL;
  }
  void *ptr = &arena->data[arena->offset];
  arena->offset += _align_up(size);
  return ptr;
}

// Duplicate the string, allocating it on the arena.
char *arena_strdup(Arena *arena, const char *s) {
  size_t len = strlen(s);
  char *new_s = arena_alloc(arena, len + 1);
  if (new_s == NULL) {
    return NULL;
  }
  memcpy(new_s, s, len + 1);
  return new_s;
}

// Reset the arena, making pointers to all previously allocated objects invalid.
void arena_reset(Arena *arena) { arena->offset = 0; }

// Free the memory taken by the arena.
void arena_free(Arena *arena) {
  free(arena->data);
  arena->data = NULL;
}
