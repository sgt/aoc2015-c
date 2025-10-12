#pragma once

#include "common.h"
#include "data_structures.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

bs_range_op parse_cmd(char *cmd) {
  if (cmd[4] == 'l')
    return FLIP;
  if (cmd[6] == 'n')
    return SET;
  return CLEAR;
}

void day06_perform_cmd(uint8_t *arr, solution_part part, char *cmd, uint16_t x1,
                       uint16_t y1, uint16_t x2, uint16_t y2) {
  bs_range_op op = parse_cmd(cmd);
  for (size_t y = y1; y <= y2; ++y) {
    for (size_t x = x1; x <= x2; ++x) {
      uint8_t *elem = arr + x + y * 1000;
      switch (op) {
      case SET:
        if (part == PART1) {
          *elem = 1;
        } else {
          *elem += 1;
        }
        break;
      case CLEAR:
        if (part == PART1) {
          *elem = 0;
        } else {
          *elem = (*elem == 0) ? 0 : *elem - 1;
        }
        break;
      case FLIP:
        if (part == PART1) {
          *elem = (*elem == 0) ? 1 : 0;
        } else {
          *elem += 2;
        }
        break;
      }
    }
  }
}

int32_t day06_total(uint8_t *arr) {
  uint32_t result = 0;
  for (size_t i = 0; i < 1000 * 1000; ++i) {
    result += arr[i];
  }
  return result;
}

uint32_t day6(const solution_part part) {
  FILE *f = fopen("data/input06.txt", "r");
  if (f == NULL) {
    perror("error opening input file");
    return -1;
  }

  uint8_t arr[1000 * 1000] = {0};
  char line[128];
  while (fgets(line, 100, f) != NULL) {
    char cmd[32];
    uint16_t x1, y1, x2, y2;
    if (sscanf(line, "%16[a-z ] %hd,%hd through %hd,%hd\n", cmd, &x1, &y1, &x2,
               &y2) != 5) {
      fprintf(stderr, "couldn't parse line: '%s'", line);
      exit(1);
    }
    day06_perform_cmd(arr, part, cmd, x1, y1, x2, y2);
  }
  fclose(f);
  return day06_total(arr);
}

uint32_t day6_part1() { return day6(PART1); }
uint32_t day6_part2() { return day6(PART2); }
