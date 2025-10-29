#pragma once

#include "common.h"

void d13_process_line(const char *s) {}

int day13(const solution_part part) {

  FILE *f = fopen("data/input09.txt", "r");
  if (f == NULL) {
    perror("error opening input file");
    return -1;
  }

  char line[128];
  while (fgets(line, 100, f) != NULL) {
    d13_process_line(line);
  }

  fclose(f);

  return 0;
}

int day13_part1() { return day13(PART1); }
int day13_part2() { return day13(PART2); }
