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

#define DS_INITIAL_CAPACITY (2 ^ 10) // must be power of 2
#define DS_GROW_FACTOR 2             // also must be power of 2

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

// Number of elements in the array.
#define arr_len(arr) ((arr) ? _arr_header(arr)->len : 0)

// Dispose of the array.
#define arr_free(arr)                                                          \
  {                                                                            \
    if (arr) {                                                                 \
      free(_arr_header(arr));                                                  \
    }                                                                          \
    (arr) = NULL;                                                              \
  }

// Push value to the end of the array, growing its capacity if needed.
#define arr_push(arr, val)                                                     \
  {                                                                            \
    (arr) = _arr_grow_if_needed(arr, sizeof(*(arr)));                          \
    (arr)[_arr_header(arr)->len++] = (val);                                    \
  }

// Return the last value of the array, evicting it from the array.
#define arr_pop(arr) (arr)[--(_arr_header(arr)->len)]

// Return the last value of the array.
#define arr_last(arr) (arr)[_arr_header(arr)->len - 1]

// ==== Hash Table (append only) ====

// Variable naming: arr refers to user-visible dynamic array of items, ht refers
// to the hidden hashtable structure pointed to in the array's header.

// The hashtable header, followed by a cap-length bucket array (both allocated
// at once).
typedef struct {
  size_t cap;
} _HTBucketsHeader;

typedef struct {
  uint64_t hash;
  ptrdiff_t idx;
} _HTBucket;

// FNV hash
uint64_t hash(const void *data, size_t size) {
  uint64_t hash = 14695981039346656037ULL;
  unsigned char *bytes = (unsigned char *)data;
  for (size_t i = 0; i < size; ++i) {
    hash ^= bytes[i];
    hash *= 1099511628211ULL;
  }
  return hash;
}

// Create new empty hashtable.
void *_ht_new(size_t cap) {
  // allocate header and items array right after the header
  _HTBucketsHeader *hdr =
      malloc(sizeof(_HTBucketsHeader) + sizeof(_HTBucket) * cap);
  hdr->cap = cap;
  _HTBucket *buckets = (_HTBucket *)(hdr + 1);
  for (size_t i = 0; i < cap; ++i) {
    *(buckets + i) = (_HTBucket){.hash = 0, .idx = -1};
  }
  return hdr;
}

// Find bucket index to start the search for index from.
static inline ptrdiff_t _ht_bucket_starting_idx(_HTBucketsHeader *hdr,
                                                uint64_t hash) {
  return hash & (hdr->cap - 1);
}

// Find index of bucket where the hash should be stored.
ptrdiff_t _ht_find_bucket_idx(_HTBucketsHeader *hdr, uint64_t hash) {
  _HTBucket *buckets = (_HTBucket *)(hdr + 1);
  ptrdiff_t bucket_idx = _ht_bucket_starting_idx(hdr, hash);
  while (buckets[bucket_idx].hash != hash && buckets[bucket_idx].idx != -1) {
    // advance bucket, wrapping at cap
    if (++bucket_idx == hdr->cap) {
      bucket_idx = 0;
    }
  }
  return bucket_idx;
}

// Get idx for key-value arr from appropriate bucket (or -1 if hash not in
// buckets).
ptrdiff_t _ht_get_idx_from_bucket(_HTBucketsHeader *hdr, uint64_t hash) {
  _HTBucket *buckets = (_HTBucket *)(hdr + 1);
  ptrdiff_t b_idx = _ht_find_bucket_idx(hdr, hash);
  return buckets[b_idx].idx;
}

void _ht_put_in_bucket(_HTBucketsHeader *hdr, uint64_t hash, ptrdiff_t idx) {
  _HTBucket *buckets = (_HTBucket *)(hdr + 1);
  ptrdiff_t b_idx = _ht_find_bucket_idx(hdr, hash);
  buckets[b_idx] = (_HTBucket){.hash = hash, .idx = idx};
}

// Return new grown hashtable by grow factor, redistributing existing buckets.
// The hashtable passed as argument is freed.
void *_ht_buckets_grow(_HTBucketsHeader *b_hdr, int grow_factor) {
  _HTBucketsHeader *new_buckets_hdr = _ht_new(b_hdr->cap * grow_factor);
  _HTBucket *old_buckets = (_HTBucket *)(b_hdr + 1);

  for (size_t i = 0; i < b_hdr->cap; ++i) {
    _HTBucket *b = &old_buckets[i];
    if (b->idx > -1) {
      _ht_put_in_bucket(new_buckets_hdr, b->hash, b->idx);
    }
  }

  free(b_hdr);
  return new_buckets_hdr;
}

void _ht_buckets_grow_if_needed(_ArrHeader *arr_hdr, size_t size,
                                int grow_factor) {
  if (arr_hdr->hashtable == NULL) {
    arr_hdr->hashtable = _ht_new(DS_INITIAL_CAPACITY);
  }

  if (((_HTBucketsHeader *)arr_hdr->hashtable)->cap < size * grow_factor) {
    // grow hashtable and redistribute items (silly not-in-place algorithm TBC?)
    arr_hdr->hashtable = _ht_buckets_grow(arr_hdr->hashtable, grow_factor);
  }
}

#define _ht_header(arr) ((_HTBucketsHeader *)(_arr_header(arr)->hashtable))

#define _key_hash(k) hash(&(k), sizeof(k))

// Number of elements in hashtable.
#define ht_size(arr) arr_len(arr)

// Get the index into the array where the key-value pair corresponding to the
// key is stored.
#define ht_get_idx(arr, k)                                                     \
  ((arr) ? _ht_get_idx_from_bucket(_ht_header(arr), _key_hash(k)) : -1)

// Get the value corresponding to the key.
#define ht_get(arr, k) ((arr)[ht_get_idx(arr, k)].value)

// True if the hashtable contains a value associated with the key.
#define ht_has(arr, k) ((arr) != NULL && ht_get_idx((arr), (k)) >= 0)

// Put or update a value corresponding to the given key.
#define ht_put(arr, k, v)                                                      \
  {                                                                            \
    bool new_key;                                                              \
    uint64_t h = _key_hash(k);                                                 \
    if (arr) {                                                                 \
      ptrdiff_t idx = _ht_get_idx_from_bucket(_ht_header(arr), h);             \
      if (idx >= 0) {                                                          \
        (arr)[idx].value = (v);                                                \
        new_key = false;                                                       \
      } else {                                                                 \
        new_key = true;                                                        \
      }                                                                        \
    } else {                                                                   \
      new_key = true;                                                          \
    }                                                                          \
    if (new_key) {                                                             \
      typeof(*arr) item = (typeof(*arr)){.key = (k), .value = (v)};            \
      arr_push((arr), item);                                                   \
      _ht_buckets_grow_if_needed(_arr_header(arr), ht_size(arr),               \
                                 DS_GROW_FACTOR);                              \
      _ht_put_in_bucket(_arr_header(arr)->hashtable, h, ht_size(arr) - 1);     \
    }                                                                          \
  }

// Dispose of the hashtable.
#define ht_free(arr)                                                           \
  {                                                                            \
    if (arr) {                                                                 \
      _HTBucketsHeader *b_hdr = (_arr_header(arr))->hashtable;                 \
      if (b_hdr) {                                                             \
        free(b_hdr);                                                           \
        b_hdr = NULL;                                                          \
      }                                                                        \
    }                                                                          \
    arr_free(arr);                                                             \
  }

// ==== Bitset ====

typedef struct {
  size_t capacity; // bytes allocated for bitset, right after this header
} bitset;

size_t _next_power_of_2(size_t n) { return (n >> 1) << 2; }

bitset *bitset_create(size_t capacity) {
  bitset *bs = calloc(sizeof(bitset) + capacity, 1);
  bs->capacity = capacity;
  return bs;
}

static inline uint8_t *_bs_bytes_array(bitset *bs) {
  return (uint8_t *)(bs + 1);
}

static inline size_t _bs_byte_idx(size_t idx) { return idx >> 3; }

// Get pointer to byte containing bit number idx, or NULL if idx is over bitset
// capacity.
uint8_t *_bitset_byte(bitset *bs, size_t idx) {
  size_t byte_idx = _bs_byte_idx(idx);
  if (byte_idx > (bs->capacity - 1)) {
    return NULL;
  }
  uint8_t *bytes = _bs_bytes_array(bs);
  return &bytes[byte_idx];
}

bool bitset_get(bitset *bs, size_t idx) {
  uint8_t *byte = _bitset_byte(bs, idx);
  if (byte == NULL)
    return false;
  uint8_t bit_idx = idx & 7;
  return *byte & (1 << bit_idx);
}

bitset *_bitset_grow(bitset *bs, size_t min_cap) {
  size_t new_cap = min_cap * DS_GROW_FACTOR;
  bs = realloc(bs, sizeof(bitset) + new_cap);
  memset(_bs_bytes_array(bs) + bs->capacity, 0, new_cap - bs->capacity);
  bs->capacity = new_cap;
  return bs;
}

void bitset_set(bitset **bs, size_t idx) {
  uint8_t *byte = _bitset_byte(*bs, idx);
  if (byte == NULL) {
    *bs = _bitset_grow(*bs, (idx >> 3) + 1);
    byte = _bitset_byte(*bs, idx);
    if (byte == NULL) {
      perror("can't grow bitset");
      exit(1);
    }
  }
  uint8_t bit_idx = idx & 7;
  *byte |= 1 << bit_idx;
}

void bitset_flip(bitset **bs, size_t idx) {
  uint8_t *byte = _bitset_byte(*bs, idx);
  if (byte == NULL) {
    *bs = _bitset_grow(*bs, (idx >> 3) + 1);
    byte = _bitset_byte(*bs, idx);
    if (byte == NULL) {
      perror("can't grow bitset");
      exit(1);
    }
  }
  uint8_t bit_idx = idx & 7;
  *byte ^= 1 << bit_idx;
}

#if defined(_MSC_VER)
#include <intrin.h>
#endif

static inline uint8_t popcount8(uint8_t x) {
#if defined(_MSC_VER)
#pragma intrinsic(__popcnt)
  return __popcnt(x);
#elif defined(__GNUC__) || defined(__clang__)
  return __builtin_popcount(x);
#else
  x = x - ((x >> 1) & 0x55);          // Count bits in each 2-bit group
  x = (x & 0x33) + ((x >> 2) & 0x33); // Sum into 4-bit groups
  x = (x + (x >> 4)) & 0x0F;          // Sum into 8-bit group, mask to 4 bits
  return x;                           // Since result fits in low 4 bits
#endif
}

size_t bitset_cardinality(bitset *bs) {
  uint8_t *bytes = _bs_bytes_array(bs);
  size_t total = 0;
  for (size_t i = 0; i < bs->capacity; ++i) {
    total += popcount8(bytes[i]); // room for improvement, I know!
  }
  return total;
}

void bitset_free(bitset *bs) {
  free(bs);
  bs = NULL;
}
