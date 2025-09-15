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

bool move(vec2 *pos, int c) {
  switch (c) {
  case '^':
    *pos = (vec2){pos->x, pos->y - 1};
    return true;
  case 'v':
    *pos = (vec2){pos->x, pos->y + 1};
    return true;
  case '>':
    *pos = (vec2){pos->x + 1, pos->y};
    return true;
  case '<':
    *pos = (vec2){pos->x - 1, pos->y};
    return true;
  default:
    return false;
  }
}

uint32_t day3(const solution_part part) {
  FILE *f = fopen("data/input03.txt", "r");
  if (f == NULL) {
    perror("error opening input file");
    return -1;
  }

  house *houses = NULL;
  bool robo_santa = false;
  vec2 santa_pos = {0, 0};
  vec2 robo_pos = {0, 0};
  vec2 *pos = &santa_pos;
  inc_house(&houses, *pos);

  while (true) {
    int c = fgetc(f);
    if (c == EOF) {
      break;
    }
    if (move(pos, c)) {
      inc_house(&houses, *pos);
    }
    if (part == PART2) {
      pos = robo_santa ? &santa_pos : &robo_pos;
      robo_santa = !robo_santa;
    }
  }
  fclose(f);
  return ht_size(houses);
}

uint32_t day3_part1() { return day3(PART1); }
uint32_t day3_part2() { return day3(PART2); }
