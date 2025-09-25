/*
Toy data structures that I implemented just for the fun of it.

For real purposes, use Sean Barret's stb_ds.h (which this toy code is inspired
by).

For examples of usage, see test.c
*/

#pragma once

#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS 1
#define strdup _strdup
#endif

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define DS_INITIAL_CAPACITY 16 // must be power of 2
#define DS_GROW_FACTOR 2       // also must be power of 2

// ==== Dynamic Array (append only) ====

typedef struct _HTBucketsHeader _HTBucketsHeader;

// The header precedes the array pointer returned to the user, stb_ds.h style.
typedef struct {
  size_t len;
  size_t cap;
  _HTBucketsHeader *hashtable;
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
  do {                                                                         \
    if (arr) {                                                                 \
      free(_arr_header(arr));                                                  \
    }                                                                          \
    (arr) = NULL;                                                              \
  } while (0)

// Push value to the end of the array, growing its capacity if needed.
#define arr_push(arr, val)                                                     \
  do {                                                                         \
    (arr) = _arr_grow_if_needed(arr, sizeof(*(arr)));                          \
    (arr)[_arr_header(arr)->len++] = (val);                                    \
  } while (0)

// Return the last value of the array, evicting it from the array.
#define arr_pop(arr) (arr)[--(_arr_header(arr)->len)]

// Return the last value of the array.
#define arr_last(arr) (arr)[_arr_header(arr)->len - 1]

// ==== Hash Table (append only) ====

// Variable naming: arr refers to user-visible dynamic array of items, ht refers
// to the hidden hashtable structure pointed to in the array's header.

// The hashtable header, followed by a cap-length bucket array (both allocated
// at once).
struct _HTBucketsHeader {
  size_t cap;
};

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

  if (size * 4 > arr_hdr->hashtable->cap * 3) {
    // grow hashtable and redistribute items (silly not-in-place algorithm TBC?)
    arr_hdr->hashtable = _ht_buckets_grow(arr_hdr->hashtable, grow_factor);
  }
}

#define _ht_header(arr) (_arr_header(arr)->hashtable)

#define _key_hash(k) hash(&(k), sizeof(k))

// Number of elements in hashtable.
#define ht_size(arr) arr_len(arr)

// Get the index into the array where the key-value pair corresponding to the
// key is stored.
#define ht_get_idx(arr, k)                                                     \
  ((arr) ? _ht_get_idx_from_bucket(_ht_header(arr), _key_hash(k)) : -1)

// Get the value corresponding to the key.
#define ht_get(arr, k) ((arr)[ht_get_idx((arr), (k))].value)

#define ht_get_or(arr, k, def)                                                 \
  (ht_get_idx((arr), (k)) == -1 ? (def) : ht_get((arr), (k)))

// True if the hashtable contains a value associated with the key.
#define ht_has(arr, k) ((arr) != NULL && ht_get_idx((arr), (k)) >= 0)

// Put or update a value corresponding to the given key.
#define ht_put(arr, k, v)                                                      \
  do {                                                                         \
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
  } while (0)

// Dispose of the hashtable.
#define ht_free(arr)                                                           \
  do {                                                                         \
    if (arr) {                                                                 \
      _HTBucketsHeader *b_hdr = (_arr_header(arr))->hashtable;                 \
      if (b_hdr) {                                                             \
        free(b_hdr);                                                           \
        b_hdr = NULL;                                                          \
      }                                                                        \
                                                                               \
      arr_free(arr);                                                           \
    }                                                                          \
  } while (0)

// ==== String Hashtable ====
// (hash table with C-style string as key )
// copies and owns its keys

// FNV hash for strings
uint64_t hash_string(const char *data) {
  uint64_t hash = 14695981039346656037ULL;
  for (auto i = 0; data[i] != '\0'; ++i) {
    hash ^= data[i];
    hash *= 1099511628211ULL;
  }
  return hash;
}

#define sht_get_idx(arr, k)                                                    \
  ((arr) ? _ht_get_idx_from_bucket(_ht_header(arr), hash_string(k)) : -1)

#define sht_get(arr, k) ((arr)[sht_get_idx((arr), (k))].value)

#define sht_get_or(arr, k, def)                                                \
  (sht_get_idx((arr), (k)) == -1 ? (def) : sht_get((arr), (k)))

#define sht_has(arr, k) ((arr) != NULL && sht_get_idx((arr), (k)) >= 0)

#define sht_size ht_size

#define sht_put(arr, k, v)                                                     \
  do {                                                                         \
    bool new_key;                                                              \
    uint64_t h = hash_string(k);                                               \
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
      char *key_copy = strdup(k);                                              \
      if (key_copy == NULL) {                                                  \
        perror("memory allocation error");                                     \
        exit(1);                                                               \
      }                                                                        \
      typeof(*arr) item = (typeof(*arr)){.key = (key_copy), .value = (v)};     \
      arr_push((arr), item);                                                   \
      _ht_buckets_grow_if_needed(_arr_header(arr), ht_size(arr),               \
                                 DS_GROW_FACTOR);                              \
      _ht_put_in_bucket(_arr_header(arr)->hashtable, h, ht_size(arr) - 1);     \
    }                                                                          \
  } while (0)

#define sht_free(arr)                                                          \
  do {                                                                         \
    if (arr) {                                                                 \
      for (auto i = 0; i < sht_size(arr); ++i) {                               \
        free(arr[i].key);                                                      \
      }                                                                        \
      _HTBucketsHeader *b_hdr = (_arr_header(arr))->hashtable;                 \
      if (b_hdr) {                                                             \
        free(b_hdr);                                                           \
        b_hdr = NULL;                                                          \
      }                                                                        \
      arr_free(arr);                                                           \
    }                                                                          \
  } while (0)

// ==== Bitset ====

typedef struct {
  size_t u8_capacity; // bytes allocated for bitset, right after this header
} bitset;

// Get index of byte containing specified bit.
static inline size_t _bs_u8_idx(size_t u1_idx) { return u1_idx >> 3; }

// Create new bitset with at least specified capacity in bits.
bitset *bitset_create(size_t u1_capacity) {
  size_t u8_capacity = _bs_u8_idx(u1_capacity) + 1;
  bitset *bs = calloc(sizeof(bitset) + u8_capacity, 1);
  bs->u8_capacity = u8_capacity;
  return bs;
}

// Get pointer to array containing data bytes.
static inline uint8_t *_bs_bytes_array(bitset *bs) {
  return (uint8_t *)(bs + 1);
}

static inline bool _bs_is_byte_idx_beyond_capacity(bitset *bs, size_t u8_idx) {
  return u8_idx > (bs->u8_capacity - 1);
}

// min_cap is in bits
bitset *_bitset_grow(bitset *bs, size_t u1_min_cap) {
  size_t u8_min_cap = _bs_u8_idx(u1_min_cap) + 1;
  size_t u8_new_cap = u8_min_cap * DS_GROW_FACTOR;
  bs = realloc(bs, sizeof(bitset) + u8_new_cap);
  memset(_bs_bytes_array(bs) + bs->u8_capacity, 0,
         u8_new_cap - bs->u8_capacity);
  bs->u8_capacity = u8_new_cap;
  return bs;
}

// Get pointer to byte containing bit number idx, NULL if beyond capacity.
uint8_t *_bitset_byte(bitset *bs, size_t u1_idx) {
  size_t byte_idx = _bs_u8_idx(u1_idx);
  if (_bs_is_byte_idx_beyond_capacity(bs, byte_idx)) {
    return NULL;
  }
  uint8_t *bytes = _bs_bytes_array(bs);
  return &bytes[byte_idx];
}

// Get specified bit's value.
bool bitset_get(bitset *bs, size_t u1_idx) {
  if (_bs_is_byte_idx_beyond_capacity(bs, _bs_u8_idx(u1_idx))) {
    // it's zero anyway
    return false;
  }
  uint8_t *byte = _bitset_byte(bs, u1_idx);
  uint8_t u1_local_idx = u1_idx % 8;
  return *byte & (1 << u1_local_idx);
}

// Set specified bit's value to 1, mutating the bitset.
void _bitset_set(bitset **bs, size_t u1_idx) {
  uint8_t *byte = _bitset_byte(*bs, u1_idx);
  if (byte == NULL) {
    *bs = _bitset_grow(*bs, u1_idx + 1);
    byte = _bitset_byte(*bs, u1_idx);
  }
  uint8_t u1_local_idx = u1_idx % 8;
  *byte |= 1 << u1_local_idx;
}

// Set specified bit's value to 1.
#define bitset_set(bs, u1_idx) _bitset_set(&(bs), (u1_idx))

// Set specified bit's value to 0.
void bitset_clear(bitset *bs, size_t u1_idx) {
  if (_bs_is_byte_idx_beyond_capacity(bs, _bs_u8_idx(u1_idx))) {
    // it's zero anyway
    return;
  }
  uint8_t *byte = _bitset_byte(bs, u1_idx);
  uint8_t u1_local_idx = u1_idx % 8;
  *byte &= ~(1 << u1_local_idx);
}

// Flip specified bit's value 0 => 1 => 0, mutating the bitset.
void _bitset_flip(bitset **bs, size_t u1_idx) {
  uint8_t *byte = _bitset_byte(*bs, u1_idx);
  if (byte == NULL) {
    *bs = _bitset_grow(*bs, u1_idx + 1);
    byte = _bitset_byte(*bs, u1_idx);
  }
  uint8_t bit_idx = u1_idx % 8;
  *byte ^= 1 << bit_idx;
}

// Flip specified bit's value 0 => 1 => 0, mutating the bitset.
#define bitset_flip(bs, u1_idx) _bitset_flip(&(bs), (u1_idx))

#if defined(_MSC_VER)
#include <intrin.h>
#endif

static inline uint8_t _popcount8(uint8_t x) {
#if defined(_MSC_VER)
#pragma intrinsic(__popcnt)
  return __popcnt(x);
#elif defined(__GNUC__) || defined(__clang__)
  return __builtin_popcount(x);
#else
  static const uint8_t _BIT_COUNT[16] = {0, 1, 1, 2, 1, 2, 2, 3,
                                         1, 2, 2, 3, 2, 3, 3, 4};
  return _BIT_COUNT[x & 0x0f] + _BIT_COUNT[x >> 4];
#endif
}

// Return number of set bits in whole bitset.
size_t bitset_cardinality(bitset *bs) {
  uint8_t *bytes = _bs_bytes_array(bs);
  size_t total = 0;
  for (size_t i = 0; i < bs->u8_capacity; ++i) {
    total += _popcount8(bytes[i]); // room for improvement, I know!
  }
  return total;
}

// Dispose of bitset.
#define bitset_free(bs)                                                        \
  do {                                                                         \
    free(bs);                                                                  \
    (bs) = NULL;                                                               \
  } while (0)

typedef enum { SET, CLEAR, FLIP } bs_range_op;

// Set a contiguous range of bits to 1.
void _bitset_range_do(const bs_range_op op, bitset **bs, const size_t bit_idx,
                      const size_t length) {
  if (length < 1)
    return;

  size_t last_bit_idx = bit_idx + length - 1;
  size_t start_byte_idx = _bs_u8_idx(bit_idx);
  size_t last_byte_idx = _bs_u8_idx(last_bit_idx);
  if (_bs_is_byte_idx_beyond_capacity(*bs, last_byte_idx)) {
    *bs = _bitset_grow(*bs, last_bit_idx + 1);
  }

  uint8_t *bytes = _bs_bytes_array(*bs);

  // all within one byte
  if (start_byte_idx == last_byte_idx) {
    // TODO set one by one, too lazy to whip up a bitwise op
    for (int i = bit_idx; i <= last_bit_idx; ++i) {
      switch (op) {
      case SET:
        bitset_set(*bs, i);
        break;
      case CLEAR:
        bitset_clear(*bs, i);
        break;
      case FLIP:
        bitset_flip(*bs, i);
        break;
      }
    }
    return;
  }

  // first byte
  uint8_t first_bit_in_first_byte = bit_idx % 8;
  switch (op) {
  case SET:
    uint8_t first_byte_mask = 0xff << first_bit_in_first_byte;
    bytes[start_byte_idx] |= first_byte_mask;
    break;
  case CLEAR:
    first_byte_mask = ~(0xff << first_bit_in_first_byte);
    bytes[start_byte_idx] &= first_byte_mask;
    break;
  case FLIP:
    first_byte_mask = 0xff << first_bit_in_first_byte;
    bytes[start_byte_idx] ^= first_byte_mask;
    break;
  }

  // middle bytes
  for (size_t i = start_byte_idx + 1; i < last_byte_idx; ++i) {
    switch (op) {
    case SET:
      bytes[i] = 0xff;
      break;
    case CLEAR:
      bytes[i] = 0x00;
      break;
    case FLIP:
      bytes[i] = ~bytes[i];
      break;
    }
  }

  // last_byte
  uint8_t last_bit_in_last_byte = last_bit_idx % 8;
  switch (op) {
  case SET:
    uint8_t last_byte_mask = 0xff >> (7 - last_bit_in_last_byte);
    bytes[last_byte_idx] |= last_byte_mask;
    break;
  case CLEAR:
    last_byte_mask = ~(0xff >> (7 - last_bit_in_last_byte));
    bytes[last_byte_idx] &= last_byte_mask;
    break;
  case FLIP:
    last_byte_mask = 0xff >> (7 - last_bit_in_last_byte);
    bytes[last_byte_idx] ^= last_byte_mask;
    break;
  }
}

#define bitset_range_set(bs, bit_idx, length)                                  \
  _bitset_range_do(SET, &(bs), (bit_idx), (length))

#define bitset_range_clear(bs, bit_idx, length)                                \
  _bitset_range_do(CLEAR, &(bs), (bit_idx), (length))

#define bitset_range_flip(bs, bit_idx, length)                                 \
  _bitset_range_do(FLIP, &(bs), (bit_idx), (length))

