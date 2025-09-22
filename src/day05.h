#pragma once

#include "common.h"
#include "data_structures.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

static inline bool is_vowel(char c) {
  return c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u';
}

static inline bool forbidden_combo(char prev, char cur) {
  return (prev == 'a' && cur == 'b') || (prev == 'c' && cur == 'd') ||
         (prev == 'p' && cur == 'q') || (prev == 'x' && cur == 'y');
}

bool is_nice(const char *s) {
  int vowel_counter = 0;
  bool double_letter_encountered = false;
  bool forbidden_combo_encountered = false;
  char prev = '\0';
  char c;
  for (int i = 0; (c = s[i], c != '\0' && c != '\n'); ++i) {
    if (is_vowel(c)) {
      ++vowel_counter;
    }
    if (prev == c) {
      double_letter_encountered = true;
    }
    if (forbidden_combo(prev, c)) {
      forbidden_combo_encountered = true;
    }
    prev = c;
  }

  return vowel_counter >= 3 && double_letter_encountered &&
         !forbidden_combo_encountered;
}

bool is_nice2(const char *s) {
  typedef struct {
    char c1, c2;
  } pair;
  struct {
    pair key;
    int value;
  } *m = NULL;
  bool contains_two_pairs = false;
  bool contains_xyx = false;
  char prevprev = '\0';
  char prev = s[0];
  char c;
  for (int i = 1; (c = s[i], c != '\0' && c != '\n'); ++i) {
    if (!contains_xyx && prevprev == c) {
      contains_xyx = true;
    }

    if (!contains_two_pairs) {
      pair p = {prev, c};
      ptrdiff_t pair_idx = ht_get_idx(m, p);
      if (pair_idx > -1 && (i - m[pair_idx].value) > 1) {
        contains_two_pairs = true;
      } else if (pair_idx == -1) {
        ht_put(m, p, i);
      }
    }
    prevprev = prev;
    prev = c;
  }

  ht_free(m);

  return contains_two_pairs && contains_xyx;
}

uint32_t day5(const solution_part part) {
  FILE *f = fopen("data/input05.txt", "r");
  if (f == NULL) {
    perror("error opening input file");
    return -1;
  }
  char line[128];
  int result = 0;
  while (fgets(line, 100, f) != NULL) {
    if (part == PART1 ? is_nice(line) : is_nice2(line)) {
      ++result;
    }
  }
  fclose(f);
  return result;
}

uint32_t day5_part1() { return day5(PART1); }
uint32_t day5_part2() { return day5(PART2); }
