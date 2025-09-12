/*
Toy data structures that I implemented just for the fun of it.

For real purposes, use Sean Barret's stb_ds.h (which this toy code is inspired
by).

For usage examples, see test.c
*/

#pragma once

#include <stdlib.h>

#define ARR_INITIAL_CAP 16
#define ARR_GROW_FACTOR 2

typedef struct {
  size_t len;
  size_t cap;
} _ArrHeader;

#define _arr_header(arr) ((_ArrHeader *)(arr) - 1)

#define _arr_grow_if_needed(arr)                                               \
  {                                                                            \
    _ArrHeader *hdr;                                                           \
    if (arr) {                                                                 \
      hdr = _arr_header(arr);                                                  \
      if (hdr->len == hdr->cap) {                                              \
        hdr->cap *= ARR_GROW_FACTOR;                                           \
        hdr = (_ArrHeader *)realloc(hdr, sizeof(_ArrHeader) +                  \
                                             hdr->cap * sizeof(*(arr)));       \
      }                                                                        \
    } else {                                                                   \
      hdr = malloc(sizeof(_ArrHeader) +                                        \
                   sizeof(*(arr)) * ARR_INITIAL_CAP);                         \
      *hdr = (_ArrHeader){.len = 0, .cap = ARR_INITIAL_CAP};                   \
    }                                                                          \
    (arr) = (void *)(hdr + 1);                                                 \
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
    _arr_grow_if_needed(arr);                                                  \
    (arr)[_arr_header(arr)->len++] = (val);                                    \
  }

#define arr_pop(arr) (arr)[--(_arr_header(arr)->len)]

#define arr_last(arr) (arr)[_arr_header(arr)->len - 1]
