#pragma once

#include "common.h"
#include "data_structures.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct {
  enum { D7_VAR, D7_NUM } tag;
  union {
    char *var;
    uint16_t num;
  } val;
} d7_value;

static inline d7_value d7_value_var(char *s) {
  return (d7_value){.tag = D7_VAR, .val = {.var = s}};
}
static inline d7_value d7_value_num(uint16_t n) {
  return (d7_value){.tag = D7_NUM, .val = {.num = n}};
}

typedef struct {
  enum { D7_ASSIGN, D7_AND, D7_OR, D7_LSHIFT, D7_RSHIFT, D7_NOT } tag;
  d7_value v1, v2;
} d7_op;

static inline d7_op_assign() {
  return (d7_op){.tag = D7_ASSIGN, .v1 = v}
}

typedef struct {
  char *key;
  d7_op value;
} day07_elem;

static inline d7_value d7_parse_value(day07_elem *m, char *s) {
  return isdigit(s[0]) ? d7_value_num(atoi(s)) : d7_value_var(s);
}

void day07_process_line(day07_elem **m, char *line) {
  char s1[8], s2[8], s3[8];
  if (sscanf(line, "%s -> %s", s1, s2) == 2) {
    auto v = d7_parse_value(*m, s1);
    sht_put(*m, s2, v);
    return;
  }
  if (sscanf(line, "NOT %s -> %s", s1, s2) == 2) {
    auto v = d7_parse_value(*m, s1);
    sht_put(*m, s2, ~v);
    return;
  }
  if (sscanf(line, "%s AND %s -> %s", s1, s2, s3) == 3) {
    auto v = d7_parse_value(*m, s1);
    auto v2 = d7_parse_value(*m, s2);
    sht_put(*m, s3, v & v2);
    return;
  }
  if (sscanf(line, "%s OR %s -> %s", s1, s2, s3) == 3) {
    auto v = d7_parse_value(*m, s1);
    auto v2 = d7_parse_value(*m, s2);
    sht_put(*m, s3, v | v2);
    return;
  }
  if (sscanf(line, "%s LSHIFT %s -> %s", s1, s2, s3) == 3) {
    auto v = d7_parse_value(*m, s1);
    auto v2 = d7_parse_value(*m, s2);
    sht_put(*m, s3, v << v2);
    return;
  }
  if (sscanf(line, "%s RSHIFT %s -> %s", s1, s2, s3) == 3) {
    auto v = d7_parse_value(*m, s1);
    auto v2 = d7_parse_value(*m, s2);
    sht_put(*m, s3, v >> v2);
    return;
  }

  fprintf(stderr, "couldn't parse line '%s'", line);
  exit(1);
}

uint32_t day7(const solution_part part) {
  FILE *f = fopen("data/input07.txt", "r");
  if (f == NULL) {
    perror("error opening input file");
    return -1;
  }

  day07_elem *m = NULL;
  char line[128];
  while (fgets(line, 100, f) != NULL) {
    day07_process_line(&m, line);
  }

  auto result = sht_get_or(m, "a", -1);
  sht_free(m);
  return result;
}

uint32_t day7_part1() { return day7(PART1); }
uint32_t day7_part2() { return day7(PART2); }
