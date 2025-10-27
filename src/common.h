#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define i8 int8_t
#define u8 uint8_t
#define i16 int16_t
#define u16 uint16_t
#define i32 int32_t
#define u32 uint32_t
#define i64 int64_t
#define u64 uint64_t
#define usize size_t
#define isize ptrdiff_t

#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS 1
#define strdup _strdup
#endif

#define min3(a, b, c) min(a, min(b, c))

typedef enum { PART1, PART2 } solution_part;

typedef struct {
  const char *input;
  const char *sep;
} splitter;

splitter splitter_new(const char *input, const char *sep) {
  return (splitter){input, sep};
}

bool splitter_next(splitter *s, size_t token_size, char *token) {
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

bool str2uint32(const char *str, uint32_t *result) {
  char *p_end;
  *result = strtol(str, &p_end, 10);
  return *p_end == '\0';
}

int _compare_uint32(const void *a, const void *b) {
  const uint32_t arg1 = *(const uint32_t *)a;
  const uint32_t arg2 = *(const uint32_t *)b;
  if (arg1 < arg2)
    return -1;
  if (arg1 > arg2)
    return 1;
  return 0;
}

void sort_uint32_array(uint32_t *arr, size_t size) {
  qsort(arr, size, sizeof(uint32_t), _compare_uint32);
}

typedef struct {
  int32_t x, y;
} vec2;

typedef struct {
  int32_t x, y, z;
} vec3;

vec3 vec3_sorted(vec3 v) {
  uint32_t sorted_sides[3] = {v.x, v.y, v.z};
  sort_uint32_array(&sorted_sides[0], 3);
  return (vec3){sorted_sides[0], sorted_sides[1], sorted_sides[2]};
}
