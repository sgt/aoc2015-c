/*
Toy data structures that I implemented just for the fun of it.

For real purposes, use Sean Barret's stb_ds.h (by which this toy code is
inspired).

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

static long *_arr_create() {
  void *hdr = malloc(sizeof(_ArrHeader) + sizeof(long) * ARR_INITIAL_CAP);
  *(_ArrHeader *)hdr = (_ArrHeader){.len = 0, .cap = ARR_INITIAL_CAP};
  return (long *)((char *)hdr + sizeof(_ArrHeader));
}

static _ArrHeader *_arr_header(long **arr) {
  if (*arr == NULL) {
    *arr = _arr_create();
  }
  return (_ArrHeader *)((char *)(*arr) - sizeof(_ArrHeader));
}

static size_t arr_len(long **arr) { return _arr_header(arr)->len; }

static void arr_free(long **arr) {
  free(_arr_header(arr));
  *arr = NULL;
}

static long *arr_push(long **arr, long elem) {
  _ArrHeader *hdr = _arr_header(arr);
  if (hdr->len == hdr->cap) {
    hdr->cap *= ARR_GROW_FACTOR;
    hdr = (_ArrHeader *)realloc(hdr,
                                sizeof(_ArrHeader) + hdr->cap * sizeof(long));
  }
  *arr = (long *)((char *)hdr + sizeof(_ArrHeader));
  hdr->len++;
  (*arr)[hdr->len - 1] = elem;
  return &(*arr)[hdr->len - 1];
}

static long arr_pop(long **arr) {
  _ArrHeader *hdr = _arr_header(arr);
  if (hdr->len == 0) {
    return 0;
  }
  long elem = (*arr)[hdr->len - 1];
  hdr->len--;
  return elem;
}
