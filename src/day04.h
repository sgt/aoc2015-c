#pragma once

#include "../thirdparty/md5.h"
#include "common.h"
#include <stdbool.h>

#define INPUT "yzbqklnj"

static inline bool digest_has_five_zeroes(char *s) {
  uint8_t result[16];
  md5String(s, result);
  return result[0] == 0 && result[1] == 0 && result[2] < 0x10;
}

static inline bool digest_has_six_zeroes(char *s) {
  uint8_t result[16];
  md5String(s, result);
  return result[0] == 0 && result[1] == 0 && result[2] == 0;
}

uint32_t day4(const solution_part part) {
  char s[16];
  for (int i = 0;; i++) {
    sprintf(s, "%s%d", INPUT, i);
    if (part == PART1 ? digest_has_five_zeroes(s) : digest_has_six_zeroes(s))
      return i;
  }
  return -1;
}

uint32_t day4_part1() { return day4(PART1); }
uint32_t day4_part2() { return day4(PART2); }
