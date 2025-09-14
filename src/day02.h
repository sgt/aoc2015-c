#pragma once

#include "common.h"
#include <stdbool.h>
#include <stdio.h>

#define NUM_LEN 32
#define LINE_LEN 128

typedef struct {
  uint32_t x, y, z;
} vec3;

vec3 vec3_sorted(vec3 v) {
  uint32_t sorted_sides[3] = {v.x, v.y, v.z};
  sort_uint32_array(&sorted_sides[0], 3);
  return (vec3){sorted_sides[0], sorted_sides[1], sorted_sides[2]};
}

uint32_t day2_formula1(vec3 v) {
  vec3 sv = vec3_sorted(v);
  return 2 * sv.x * sv.y + 2 * sv.x * sv.z + 2 * sv.y * sv.z + sv.x * sv.y;
}

uint32_t day2_formula2(vec3 v) {
  vec3 sv = vec3_sorted(v);
  return 2 * sv.x + 2 * sv.y + sv.x * sv.y * sv.z;
}

uint32_t day2(const solution_part part) {
  FILE *f = fopen("data/input02.txt", "r");
  uint32_t total = 0;
  char line[LINE_LEN] = {0};
  while (fgets(line, LINE_LEN, f) != NULL) {
    vec3 vec = {0};
    if (sscanf(line, "%dx%dx%d", &vec.x, &vec.y, &vec.z) != 3) {
      fprintf(stderr, "error parsing vector: '%s'", line);
      return -1;
    }
    total += part == PART1 ? day2_formula1(vec) : day2_formula2(vec);
  }
  fclose(f);
  return total;
}

uint32_t day2_part1() { return day2(PART1); }
uint32_t day2_part2() { return day2(PART2); }
