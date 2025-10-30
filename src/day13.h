#pragma once

#include "common.h"
#include "data_structures.h"
#include <limits.h>

#define D13_MAX_PEOPLE 9

typedef struct {
  strpool pool;
  int cost[D13_MAX_PEOPLE][D13_MAX_PEOPLE];
  int max_happiness;
} d13_state;

d13_state d13_init_state() {
  return (d13_state){
      .pool = strpool_init(), .cost = {{0}}, .max_happiness = INT_MIN};
}

void d13_add_cost(d13_state *state, char *n1, char *n2, int cost) {
  const usize i1 = strpool_idx(&state->pool, n1);
  const usize i2 = strpool_idx(&state->pool, n2);
  state->cost[i1][i2] = cost;
}

void d13_process_line(d13_state *state, const char *line) {
  char n1[16], n2[16], verb[5];
  int cost;
  // Alice would gain 2 happiness units by sitting next to Bob.
  if (sscanf(line,
             "%15[a-zA-Z] would %4s %d happiness units by sitting next to "
             "%15[a-zA-Z].",
             n1, verb, &cost, n2) != 4) {
    fprintf(stderr, "couldn't parse line '%s'\n", line);
    exit(1);
  }
  d13_add_cost(state, n1, n2, verb[0] == 'g' ? cost : 0 - cost);
}

void d13_swap(int *a, int *b) {
  int temp = *a;
  *a = *b;
  *b = temp;
}

typedef void (*d13_callback_fn)(d13_state *, const int *, usize);

// Recursive implementation of Heap's algorithm
void d13_heaps_recursive(d13_state *state, int *arr, int n, usize size,
                         d13_callback_fn callback) {
  if (n == 1) {
    callback(state, arr, size);
    return;
  }

  for (int i = 0; i < n; ++i) {
    d13_heaps_recursive(state, arr, n - 1, size, callback);

    if (n % 2 == 0) {
      d13_swap(&arr[i], &arr[n - 1]);
    } else {
      d13_swap(&arr[0], &arr[n - 1]);
    }
  }
}

// TODO discard circularly equivalent permutations?
void d13_permute(d13_state *state, usize size, d13_callback_fn callback) {
  int seatings[size];
  for (int i = 0; i < size; ++i) {
    seatings[i] = i;
  }
  d13_heaps_recursive(state, seatings, size, size, callback);
}

void d13_process(d13_state *state, const int *seating, usize size) {
  int total = 0;
  for (usize i = 0; i < size; ++i) {
    const usize person = seating[i];
    const usize next_person = seating[i == size - 1 ? 0 : i + 1];
    const usize prev_person = seating[i == 0 ? size - 1 : i - 1];
    total += state->cost[person][next_person];
    total += state->cost[person][prev_person];
  }
  state->max_happiness = max(state->max_happiness, total);
}

int day13(const solution_part part) {

  FILE *f = fopen("data/input13.txt", "r");
  if (f == NULL) {
    perror("error opening input file");
    return -1;
  }

  d13_state state = d13_init_state();
  char line[128];
  while (fgets(line, 100, f) != NULL) {
    d13_process_line(&state, line);
  }

  fclose(f);

  usize people_count = strpool_len(&state.pool);

  d13_permute(&state, part == PART1 ? people_count : people_count + 1,
              d13_process);

  return state.max_happiness;
}

int day13_part1() { return day13(PART1); }
int day13_part2() { return day13(PART2); }
