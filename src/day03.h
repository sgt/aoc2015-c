#pragma once

#include "common.h"
#include "data_structures.h"
#include <stdbool.h>
#include <stdio.h>

typedef struct {
  vec2 key;
  uint32_t value;
} house;

void inc_house(house **houses, vec2 pos) {
  if (!ht_has(*houses, pos)) {
    ht_put(*houses, pos, 1);
  } else {
    ht_put(*houses, pos, ht_get(*houses, pos) + 1);
  }
}

uint32_t day3(const solution_part part) {
  FILE *f = fopen("data/input03.txt", "r");
  if (f == NULL) {
    perror("error opening input file");
    return -1;
  }

  house *houses = NULL;
  vec2 pos = {0, 0};

  while (true) {
  next_char:
    int c = fgetc(f);
    if (c == EOF) {
      break;
    }
    switch (c) {
    case '^':
      pos = (vec2){pos.x, pos.y - 1};
      break;
    case 'v':
      pos = (vec2){pos.x, pos.y + 1};
      break;
    case '>':
      pos = (vec2){pos.x + 1, pos.y};
      break;
    case '<':
      pos = (vec2){pos.x - 1, pos.y};
      break;
    default:
      goto next_char; // yay! goto!
    }
    inc_house(&houses, pos);
  }
  fclose(f);
  return ht_size(houses);
}

uint32_t day3_part1() { return day3(PART1); }
uint32_t day3_part2() { return day3(PART2); }
