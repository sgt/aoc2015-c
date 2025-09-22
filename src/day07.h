#pragma once

#include "common.h"
#include "data_structures.h"
#include <stdbool.h>
#include <stdint.h>

void process_line(char *line) {

}

uint32_t day7(const solution_part part) {
  FILE *f = fopen("data/input07.txt", "r");
  if (f == NULL) {
    perror("error opening input file");
    return -1;
  }

  char line[128];
  while (fgets(line, 100, f) != NULL) {
    process_line(m,line);
  }
  return sht_get(m, "a");
}

uint32_t day7_part1() { return day7(PART1); }
uint32_t day7_part2() { return day7(PART2); }
