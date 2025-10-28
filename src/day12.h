#pragma once

#include "../thirdparty/json.h"
#include "common.h"
#include <assert.h>
#include <ctype.h>
#include <string.h>


typedef struct {
  const char *input;
  usize idx;
} d12_num_finder;

d12_num_finder d12_init_num_finder(const char *s) {
  return (d12_num_finder){.input = s, .idx = 0};
}

bool d12_isnumchar(int c) { return c == '-' || isdigit(c); }

bool d12_next_num(d12_num_finder *finder, int *num) {
  while (!d12_isnumchar(finder->input[finder->idx]) &&
         finder->input[finder->idx] != '\0') {
    ++finder->idx;
  }
  if (finder->input[finder->idx] == '\0')
    return false;
  int start = finder->idx;
  do {
    ++finder->idx;
  } while (d12_isnumchar(finder->input[finder->idx]));
  int len = finder->idx - start;
  assert(len < 128);
  char buf[128];
  memcpy(buf, &finder->input[start], len);
  buf[len] = '\0';
  *num = atoi(buf);
  return true;
}

int day12(const solution_part part) {

  FILE *f = fopen("data/input12.txt", "r");
  if (f == NULL) {
    perror("error opening input file");
    return -1;
  }

  char line[1024 * 100];
  if (fgets(line, 1024 * 100, f) == NULL) {
    perror("error reading file");
    return -1;
  }
  fclose(f);

  int sum = 0;

  switch (part) {
  case PART1:
    // using custom parser
    d12_num_finder finder = d12_init_num_finder(line);
    int n;
    while (d12_next_num(&finder, &n)) {
      sum += n;
    }
    break;
  case PART2:
    // using third-party json parser
    break;
  }

  return sum;
}

int day12_part1() { return day12(PART1); }
int day12_part2() { return day12(PART2); }
