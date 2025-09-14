#pragma once

#include "common.h"

size_t day1(const solution_part part) {
  uint32_t n = 0;
  FILE *f = fopen("data/input01.txt", "r");
  if (f == NULL) {
    perror("error opening input file");
    return -1;
  }
  uint32_t i = 0;
  while (true) {
    int c = fgetc(f);
    if (c == EOF) {
      break;
    }
    switch (c) {
    case '(':
      n++;
      i++;
      break;
    case ')':
      n--;
      i++;
      break;
    default:
      break;
    }
    if (part == PART2 && n == -1) {
      fclose(f);
      return i;
    }
  }
  fclose(f);
  return n;
}

size_t day1_part1(void) { return day1(PART1); }
size_t day1_part2(void) { return day1(PART2); }
