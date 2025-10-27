#pragma once

#include "common.h"
#include <limits.h>
#include <stdint.h>

#define D9_MAX_CITIES 8

typedef struct {
  u8 cities_count;
  char *cities[D9_MAX_CITIES];

  u16 dist[D9_MAX_CITIES][D9_MAX_CITIES];
  u16 dp[1 << D9_MAX_CITIES][D9_MAX_CITIES];
} d9_state;

d9_state d9_init_state(void) {
  return (d9_state){.cities_count = 0, .dist = {0}};
}

// Return city's index, adding a new city if absent from index.
usize d9_city_index(d9_state *state, const char *s) {
  for (u8 i = 0; i < state->cities_count; ++i) {
    if (strcmp(s, state->cities[i]) == 0) {
      return i;
    }
  }
  if (state->cities_count == D9_MAX_CITIES) {
    fprintf(stderr, "attempting to add more than max cities, aborting");
    exit(1);
  }
  state->cities[state->cities_count] = strdup(s);
  state->cities_count++;
  return state->cities_count - 1;
}

void d9_free_state(d9_state state) {
  for (usize i = 0; i < state.cities_count; i++) {
    free(state.cities[i]);
  }
}

void d9_add_distance(d9_state *state, const char *s1, const char *s2,
                     u16 distance) {
  usize i1 = d9_city_index(state, s1);
  usize i2 = d9_city_index(state, s2);
  state->dist[i1][i2] = distance;
  state->dist[i2][i1] = distance;
}

void d9_process_line(d9_state *state, const char *line) {
  char n1[16], n2[16];
  u16 distance;
  if (sscanf(line, "%15s to %15s = %hu", n1, n2, &distance) != 3) {
    fprintf(stderr, "couldn't parse line '%s'\n", line);
    exit(1);
  }
  d9_add_distance(state, n1, n2, distance);
}

bool d9_has_city(int mask, u8 city) { return mask & (1 << city); }
int d9_remove_city(int mask, u8 city) { return mask & ~(1 << city); }

int d9_full_mask(const d9_state *state) {
  return (1 << state->cities_count) - 1;
}

u16 d9_held_karp(d9_state *state, const solution_part part) {
  int full_mask = d9_full_mask(state);

  // init dp array
  for (int mask = 0; mask <= full_mask; ++mask) {
    for (u8 city = 0; city < state->cities_count; ++city) {
      state->dp[mask][city] =
          mask == (1 << city) ? 0 : (part == PART1 ? UINT16_MAX : 0);
    }
  }

  for (int mask = 1; mask <= full_mask; ++mask) {
    for (u8 city = 0; city < state->cities_count; ++city) {
      if (!d9_has_city(mask, city))
        continue;
      for (u8 prev_city = 0; prev_city < state->cities_count; ++prev_city) {
        if (prev_city == city || !d9_has_city(mask, prev_city))
          continue;
        int prev_mask = d9_remove_city(mask, city);
        u16 current_cost =
            state->dp[prev_mask][prev_city] + state->dist[prev_city][city];
        state->dp[mask][city] = part == PART1
                                    ? min(state->dp[mask][city], current_cost)
                                    : max(state->dp[mask][city], current_cost);
        ;
      }
    }
  }

  u16 result = part == PART1 ? UINT16_MAX : 0;
  for (u8 city = 0; city < state->cities_count; ++city) {
    result = part == PART1 ? min(result, state->dp[full_mask][city])
                           : max(result, state->dp[full_mask][city]);
  }
  return result;
}

u16 day9(const solution_part part) {
  FILE *f = fopen("data/input09.txt", "r");
  if (f == NULL) {
    perror("error opening input file");
    return -1;
  }

  d9_state state = d9_init_state();
  char line[128];
  while (fgets(line, 100, f) != NULL) {
    d9_process_line(&state, line);
  }

  u16 result = d9_held_karp(&state, part);

  fclose(f);
  d9_free_state(state);

  return result;
}

u16 day9_part1() { return day9(PART1); }
u16 day9_part2() { return day9(PART2); }
