#pragma once

#include "common.h"
#include "data_structures.h"
#include <stdbool.h>
#include <stdint.h>

bs_range_op parse_cmd(char *cmd) {
  if (cmd[4] == 'l')
    return FLIP;
  if (cmd[6] == 'n')
    return SET;
  return CLEAR;
}

void day06_perform_cmd(bitset **bs, char *cmd, uint16_t x1, uint16_t y1,
                       uint16_t x2, uint16_t y2) {
  for (auto y = y1; y <= y2; ++y) {
    switch (parse_cmd(cmd)) {
    case SET:
      bitset_range_set(*bs, x1 + y * 1000, x2 - x1 + 1);
      break;
    case CLEAR:
      bitset_range_clear(*bs, x1 + y * 1000, x2 - x1 + 1);
      break;
    case FLIP:
      bitset_range_flip(*bs, x1 + y * 1000, x2 - x1 + 1);
      break;
    }
  }
}

uint32_t day6(const solution_part part) {
  FILE *f = fopen("data/input06.txt", "r");
  if (f == NULL) {
    perror("error opening input file");
    return -1;
  }

  bitset *bs = bitset_create(1000 * 1000);
  char line[128];
  while (fgets(line, 100, f) != NULL) {
    char cmd[32];
    uint16_t x1, y1, x2, y2;
    if (sscanf(line, "%16[a-z ] %hd,%hd through %hd,%hd\n", cmd, &x1, &y1, &x2,
               &y2) != 5) {
      fprintf(stderr, "couldn't parse line: '%s'", line);
      exit(1);
    }
    day06_perform_cmd(&bs, cmd, x1, y1, x2, y2);
  }
  fclose(f);

  return bitset_cardinality(bs);
}

uint32_t day6_part1() { return day6(PART1); }
uint32_t day6_part2() { return day6(PART2); }
