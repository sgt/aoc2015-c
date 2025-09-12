/*
Toy data structures that I implemented just for the fun of it.

For real purposes, use Sean Barret's stb_ds.h (which this toy code is inspired
by).

For usage examples, see test.c
*/

#pragma once

#include <stdint.h>
#include <stdlib.h>

#define DS_INITIAL_CAPACITY 16
#define DS_GROW_FACTOR 2

typedef struct {
  size_t len;
  size_t cap;
  void *hashtable;
} _ArrHeader;

typedef struct {
  size_t cap;
  void *buckets;
} _Hash_Table;

// ==== Dynamic Array ====

#define _arr_header(arr) ((_ArrHeader *)(arr) - 1)

void *_arr_grow_if_needed(void *arr, size_t elem_size) {
  _ArrHeader *hdr;
  if (arr) {
    hdr = _arr_header(arr);
    if (hdr->len == hdr->cap) {
      hdr->cap *= DS_GROW_FACTOR;
      hdr = realloc(hdr, sizeof(_ArrHeader) + hdr->cap * elem_size);
    }
  } else {
    hdr = malloc(sizeof(_ArrHeader) + elem_size * DS_INITIAL_CAPACITY);
    *hdr = (_ArrHeader){.len = 0, .cap = DS_INITIAL_CAPACITY};
  }
  return (void *)(hdr + 1);
}

#define arr_len(arr) ((arr) ? _arr_header(arr)->len : 0)

#define arr_free(arr)                                                          \
  {                                                                            \
    if (arr) {                                                                 \
      free(_arr_header(arr));                                                  \
    }                                                                          \
    (arr) = NULL;                                                              \
  }

#define arr_push(arr, val)                                                     \
  {                                                                            \
    (arr) = _arr_grow_if_needed(arr, sizeof(*(arr)));                          \
    (arr)[_arr_header(arr)->len++] = (val);                                    \
  }

#define arr_pop(arr) (arr)[--(_arr_header(arr)->len)]

#define arr_last(arr) (arr)[_arr_header(arr)->len - 1]

// ==== Hash Table ====

// FNV hash
uint64_t hash(void *data, size_t size) {
  uint64_t hash = 14695981039346656037ULL;
  unsigned char *bytes = (unsigned char *) data;
  for (size_t i = 0; i < size; i++) {
    hash ^= bytes[i];
    hash *= 1099511628211ULL;
  }
  return hash;
}

#define ht_size(ht) 0

#define ht_put(ht, key, val) {}

#define ht_has(ht, key) false

#define ht_get(ht, key) 0

#define ht_free(ht) {}
