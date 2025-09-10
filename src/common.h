#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define min3(a, b, c) min(a, min(b, c))

typedef enum { PART1, PART2 } solution_part;

typedef struct {
  const char *input;
  const char *sep;
} splitter;

static splitter splitter_new(const char *input, const char *sep) {
  return (splitter){input, sep};
}

static bool splitter_next(splitter *s, size_t token_size, char *token) {
  if (*s->input == '\0') {
    return false;
  }
  size_t len = strcspn(s->input, s->sep);
  if (len + 1 > token_size) {
    return false;
  }
  memcpy(token, s->input, len);
  token[len] = '\0';
  s->input += len;
  s->input += strspn(s->input, s->sep);
  return true;
}

static bool str2long(const char *str, long *result) {
  char *p_end;
  *result = strtol(str, &p_end, 10);
  return *p_end == '\0';
}

static int _compare_longs(const void *a, const void *b) {
  const long arg1 = *(const long *)a;
  const long arg2 = *(const long *)b;
  if (arg1 < arg2)
    return -1;
  if (arg1 > arg2)
    return 1;
  return 0;
}

static void sort_longs(long *arr, size_t size) {
  qsort(arr, size, sizeof(long), _compare_longs);
}
