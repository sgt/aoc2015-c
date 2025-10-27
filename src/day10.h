#pragma once

#include "common.h"
#include "data_structures.h"

#define PUZZLE_INPUT "1321131112"

usize d10_char_repeated(char *s, usize idx) {
  char c = s[idx];
  usize i = idx + 1;
  while (s[i] == c && s[i] != '\0') {
    ++i;
  }
  return i - idx;
}

void arr_push_str(char **arr, char *s) {
  for (usize i = 0; s[i] != '\0'; ++i) {
    arr_push(*arr, s[i]);
  }
}

char *d10_look_and_say(char *s) {
  char buf[1024];
  char *result = NULL;
  usize i = 0;
  while (s[i] != '\0') {
    usize repetitions = d10_char_repeated(s, i);
    sprintf(buf, "%zu", repetitions);
    arr_push_str(&result, buf);
    arr_push(result, s[i]);
    i += repetitions;
  }
  arr_push(result, '\0');
  return result;
}

usize day10(const solution_part part) {
  char input[1024*1024];
  strcpy(input, PUZZLE_INPUT);
  for (int i = 0; i < 40; ++i) {
    char *s = d10_look_and_say(input);
    printf("%d %zu\n", i, strlen(s));
    strcpy(input, s);
    arr_free(s);
  }
  return strlen(input);
}

usize day10_part1() { return day10(PART1); }
usize day10_part2() { return day10(PART2); }
