/*
Toy data structures that I implemented just for the fun of it.

For real purposes, use Sean Barret's stb_ds.h (which this toy code is inspired
by).

For usage examples, see test.c
*/

#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define DS_INITIAL_CAPACITY (2 ^ 4) // must be power of 2
#define DS_GROW_FACTOR 2            // also must be power of 2

// ==== Dynamic Array (append only) ====

// The header precedes the array pointer returned to the user, stb_ds.h style.
typedef struct {
  size_t len;
  size_t cap;
  void *hashtable;
} _ArrHeader;

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

// ==== Hash Table (append only) ====

// Variable naming: arr refers to user-visible dynamic array of items, ht refers
// to the hidden hashtable structure pointed to in the array's header.

// The hashtable header, followed by a cap-length bucket array (both allocated
// at once).
typedef struct {
  size_t cap;
} _HTHeader;

typedef struct {
  size_t hash;
  ptrdiff_t idx;
} _HTItem;

// FNV hash
uint64_t hash(void *data, size_t size) {
  uint64_t hash = 14695981039346656037ULL;
  unsigned char *bytes = (unsigned char *)data;
  for (size_t i = 0; i < size; i++) {
    hash ^= bytes[i];
    hash *= 1099511628211ULL;
  }
  return hash;
}

size_t _ht_bucket_idx(size_t cap, void *key, size_t key_size) {
  return hash(key, key_size) & (cap - 1);
}

// Create new empty hashtable.
void *_ht_new(size_t cap) {
  // allocate header and items array right after the header
  _HTHeader *hdr = malloc(sizeof(_HTHeader) + sizeof(_HTItem) * cap);
  hdr->cap = cap;
  _HTItem *items = (_HTItem *)(hdr + 1);
  for (size_t i = 0; i < cap; i++) {
    *(items + i) = (_HTItem){.hash = 0, .idx = -1};
  }
  return hdr;
}

// Return new grown hashtable by grow factor, redistributing existing buckets.
void *_ht_grow(_HTHeader *ht, int grow_factor) {
  _HTHeader *new_ht = _ht_new(ht->cap * grow_factor);
  _HTItem *items = (_HTItem *)(ht + 1);

  return new_ht;
}

void *_ht_grow_if_needed(_HTHeader *ht, size_t size) {
  if (ht == NULL) {
    return _ht_new(DS_INITIAL_CAPACITY);
  }

  if (ht->cap < size *) {
    // grow hashtable and redistribute items (silly not-in-place algorithm TBC?)
    _HTHeader *new_ht = _HTItem *old_items = (_HTItem *)(ht + 1);
  }
}

#define ht_size(arr) arr_len(arr)

#define ht_put(arr, key, val)                                                  \
  {                                                                            \
    arr_push(arr, (typeof(*arr)){.key=(key), .value=(val)}));                  \
    (t) = _ht_grow_if_needed(_arr_header(arr)->hashtable);                      \
  }

#define ht_get_idx(arr, key) -1

#define ht_get(arr, key) (arr[ht_get_idx(arr, key)].value)

#define ht_has(arr, key) ((ht_get_idx(arr, key)) >= 0)

#define ht_free(arr)                                                           \
  {                                                                            \
    /* TODO */                                                                 \
    arr_free(arr);                                                             \
  }
