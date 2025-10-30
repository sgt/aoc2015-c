#pragma once

#include "common.h"

#define D14_MAX_DEERS 16
#define D14_TIME_LIMIT 2503

typedef struct {
  int fly_time;
  int fly_speed;
  int rest_time;
} d14_deer;

typedef struct {
  usize deer_count;
  d14_deer deers[D14_MAX_DEERS];
} d14_context;

d14_context d14_init_context() {
  return (d14_context){.deer_count = 0, .deers = {0}};
}

void d14_add_deer(d14_context *ctx, d14_deer deer) {
  ctx->deers[ctx->deer_count] = deer;
  ++ctx->deer_count;
}

void d14_process_line(d14_context *ctx, const char *line) {
  char name[16];
  d14_deer deer;
  // Alice would gain 2 happiness units by sitting next to Bob.
  if (sscanf(line,
             "%15s can fly %d km/s for %d seconds, but then must rest for %d ",
             name, &deer.fly_speed, &deer.fly_time, &deer.rest_time) != 4) {
    fprintf(stderr, "couldn't parse line '%s'\n", line);
    exit(1);
  }
  d14_add_deer(ctx, deer);
}

int d14_distance_traveled(const d14_deer deer, const int seconds) {
  const int cycle = deer.fly_time + deer.rest_time;
  return deer.fly_speed * (seconds / cycle * deer.fly_time +
                           min(seconds % cycle, deer.fly_time));
}

int d14_max_distance_traveled(const d14_context ctx, const int seconds) {
  int max_distance = 0;
  for (int i = 0; i < ctx.deer_count; ++i) {
    max_distance =
        max(max_distance, d14_distance_traveled(ctx.deers[i], seconds));
  }
  return max_distance;
}

int d14_new_scoring_winner(const d14_context ctx, const int seconds) {
  int scores[ctx.deer_count];
  int distances[ctx.deer_count];

  for (int i = 0; i < ctx.deer_count; ++i)
    scores[i] = 0;

  for (int t = 1; t <= seconds; ++t) {
    int max_distance = 0;
    for (int i = 0; i < ctx.deer_count; ++i) {
      distances[i] = d14_distance_traveled(ctx.deers[i], t);
      max_distance = max(max_distance, distances[i]);
    }
    for (int i = 0; i < ctx.deer_count; ++i) {
      if (distances[i] == max_distance)
        scores[i] += 1;
    }
  }

  int max_score = 0;
  for (int i = 0; i < ctx.deer_count; ++i) {
    max_score = max(max_score, scores[i]);
  }
  return max_score;
}

int day14(const solution_part part) {
  FILE *f = fopen("data/input14.txt", "r");
  if (f == NULL) {
    perror("error opening input file");
    return -1;
  }

  d14_context ctx = d14_init_context();
  char line[128];
  while (fgets(line, 100, f) != NULL) {
    d14_process_line(&ctx, line);
  }

  fclose(f);

  if (part == PART1)
    return d14_max_distance_traveled(ctx, D14_TIME_LIMIT);

  return d14_new_scoring_winner(ctx, D14_TIME_LIMIT);
}

int day14_part1() { return day14(PART1); }
int day14_part2() { return day14(PART2); }
