#pragma once

#include "common.h"

// Mutates s. A quick hack, not checking boundaries or spilling over a new
// digit.
void d11_increase_pw(char *s) {
  usize len = strlen(s);
  usize i = len - 1;
  bool carry = false;
  do {
    if (s[i] == 'z') {
      carry = true;
      s[i] = 'a';
    } else {
      carry = false;
      s[i] += 1;
    }
    --i;
  } while (carry);
}

bool d11_is_valid_pw(const char *s) {
  bool three_sequential_encountered = false;
  bool two_doubles_encountered = false;
  isize prev_double_encountered = -1;
  for (usize i = 0; s[i] != '\0'; ++i) {
    if (s[i] == 'i' || s[i] == 'l' || s[i] == 'o') {
      return false;
    }

    if (!three_sequential_encountered && i >= 2 && s[i - 1] == s[i] - 1 && s[i - 2] == s[i] - 2) {
      three_sequential_encountered = true;
    }

    if (!two_doubles_encountered && i >= 1 && s[i] == s[i - 1]) {
      if (prev_double_encountered == -1) {
        prev_double_encountered = i;
      } else if (prev_double_encountered < (i - 1)) {
        two_doubles_encountered = true;
      }
    }
  }
  return three_sequential_encountered && two_doubles_encountered;
}

void d11_next_valid(char *pw) {
  while (!d11_is_valid_pw(pw))
    d11_increase_pw(pw);
}

#define D11_PUZZLE_INPUT "hepxcrrq"

char *day11(const solution_part part) {
  char *pw = strdup(D11_PUZZLE_INPUT);
  d11_next_valid(pw);
  if (part == PART1)
    return pw;
  d11_increase_pw(pw);
  d11_next_valid(pw);
  return pw;
}

char *day11_part1() { return day11(PART1); }
char *day11_part2() { return day11(PART2); }
