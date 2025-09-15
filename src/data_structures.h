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
  uint64_t hash;
  ptrdiff_t idx;
} _HTBucket;

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

// Create new empty hashtable.
void *_ht_new(size_t cap) {
  // allocate header and items array right after the header
  _HTHeader *hdr = malloc(sizeof(_HTHeader) + sizeof(_HTBucket) * cap);
  hdr->cap = cap;
  _HTBucket *buckets = (_HTBucket *)(hdr + 1);
  for (size_t i = 0; i < cap; i++) {
    *(buckets + i) = (_HTBucket){.hash = 0, .idx = -1};
  }
  return hdr;
}

inline size_t _ht_bucket_idx(_HTHeader *hdr, void *key, size_t key_size) {
  return hash(key, key_size) & (hdr->cap - 1);
}

void _ht_put_in_bucket(_HTHeader *hdr, uint64_t hash, ptrdiff_t idx) {
  _HTBucket *buckets = (_HTBucket *)(hdr + 1);
  ptrdiff_t bucket_idx = hash & (hdr->cap - 1); // hash % cap
  while (buckets[bucket_idx].idx != -1) {
    // find unused bucket, wrapping at cap
    if (++bucket_idx == hdr->cap) {
      bucket_idx = 0;
    }
  }
  buckets[bucket_idx] = (_HTBucket){.hash = hash, .idx = idx};
}

ptrdiff_t _ht_get_idx_from_bucket(_HTHeader *hdr, uint64_t hash) {
  _HTBucket *buckets = (_HTBucket *)(hdr + 1);
  ptrdiff_t bucket_idx = hash & (hdr->cap - 1); // hash % cap
  while (buckets[bucket_idx].hash != hash) {
    if (++bucket_idx == hdr->cap) {
      bucket_idx = 0;
    }
  }
  return buckets[bucket_idx].idx;
}

// Return new grown hashtable by grow factor, redistributing existing buckets.
// The hashtable passed as argument is freed.
void *_ht_grow(_HTHeader *hdr, int grow_factor) {
  _HTHeader *new_ht = _ht_new(hdr->cap * grow_factor);
  _HTBucket *buckets = (_HTBucket *)(hdr + 1);

  for (size_t i = 0; i < hdr->cap; i++) {
    _HTBucket *b = &buckets[i];
    if (b->idx > -1) {
      _ht_put_in_bucket(new_ht, b->hash, b->idx);
    }
  }

  free(hdr);
  return new_ht;
}

void _ht_grow_if_needed(_ArrHeader *arr_hdr, size_t size, int grow_factor) {
  if (arr_hdr->hashtable == NULL) {
    arr_hdr->hashtable = _ht_new(DS_INITIAL_CAPACITY);
  }

  if (((_HTHeader *)arr_hdr->hashtable)->cap < size * grow_factor) {
    // grow hashtable and redistribute items (silly not-in-place algorithm TBC?)
    arr_hdr->hashtable = _ht_grow(arr_hdr->hashtable, grow_factor);
  }
}

#define _ht_header(arr) ((_HTHeader *)(_arr_header(arr)->hashtable))

#define ht_size(arr) arr_len(arr)

#define ht_put(arr, key, val)                                                  \
  {                                                                            \
    typeof(*arr) item = (typeof(*arr)){.key = (key), .value = (val)};          \
    arr_push((arr), item);                                                     \
    _ht_grow_if_needed(_arr_header(arr), ht_size(arr), DS_GROW_FACTOR);        \
    uint64_t h = hash(&(key), sizeof(typeof((arr)->key)));                     \
    _ht_put_in_bucket(_arr_header(arr)->hashtable, h, ht_size(arr) - 1);       \
  }

// TODO extract into a function so this macro can return a value.
#define ht_get_idx(arr, key)                                                   \
  {                                                                            \
    _HTHeader *ht_hdr = _ht_header(arr);                                       \
    uint64_t h = hash(&(key), sizeof(typeof((arr)->key)));                     \
    _ht_get_index_from_bucket(ht_hdr, key_hash);                               \
  }

#define ht_get(arr, key) (arr[ht_get_idx(arr, key)].value)

#define ht_has(arr, key) ((ht_get_idx(arr, key)) >= 0)

#define ht_free(arr)                                                           \
  {                                                                            \
    free(_ht_header(arr));                                                     \
    arr_free(arr);                                                             \
  }
