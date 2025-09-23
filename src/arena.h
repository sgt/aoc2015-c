#pragma once

#include <assert.h>
#include <stdlib.h>

typedef struct {
  size_t capacity;
  size_t offset;
  void *data;
} Arena;

Arena arena_create(size_t initial_capacity) {
  void *data = malloc(initial_capacity);
  assert(data);
  return (Arena){.capacity = initial_capacity, .offset = 0, .data = data};
}

void* arena_alloc(Arena*arena,size_t size) {
  if (arena->capacity-arena->offset>size) {
    void *data = realloc(arena->data, )
  }
}
