#pragma once

typedef struct {
  size_t capacity;
  size_t offset;
  void *data;
} Arena;
