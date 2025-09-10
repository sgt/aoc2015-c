#pragma once

#include "common.h"

static size_t day1(const solution_part part) {
  int n = 0;
  FILE *f = fopen("data/input01.txt", "r");
  if (f == NULL) {
    perror("error opening input file");
    return -1;
  }
  int i = 0;
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

static size_t day1_part1(void) { return day1(PART1); }
static size_t day1_part2(void) { return day1(PART2); }
