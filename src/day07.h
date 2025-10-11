#pragma once

#include "arena.h"
#include "common.h"
#include "data_structures.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct {
  enum { D7_LITERAL, D7_INTEGER } tag;
  union {
    char *literal;
    uint16_t integer;
  } val;
} d7_value;

bool _isalnumstr(const char *s) {
  assert(s != NULL);
  for (size_t i = 0; s[i] != '\0'; ++i) {
    if (!isalnum(s[i])) {
      return false;
    }
  }
  return true;
}

static inline d7_value d7_value_literal(Arena *arena, const char *s) {
  char *s_copy = arena_strdup(arena, s);
  assert(s_copy != NULL);
  return (d7_value){.tag = D7_LITERAL, .val = {.literal = s_copy}};
}

static inline d7_value d7_value_int(uint16_t n) {
  return (d7_value){.tag = D7_INTEGER, .val = {.integer = n}};
}

static inline d7_value d7_value_parse(Arena *arena, const char *s) {
  return isdigit(s[0]) ? d7_value_int(atoi(s)) : d7_value_literal(arena, s);
}

typedef struct {
  enum { D7_ID, D7_AND, D7_OR, D7_LSHIFT, D7_RSHIFT, D7_NOT } tag;
  d7_value v1, v2;
} d7_op;

#define D7_DEFINE_OP_FUNC(Name, Tag)                                           \
  static inline d7_op d7_op_##Name(Arena *arena, const char *s1,               \
                                   const char *s2) {                           \
    return (d7_op){.tag = Tag,                                                 \
                   .v1 = d7_value_parse(arena, s1),                            \
                   .v2 = d7_value_parse(arena, s2)};                           \
  }

static inline d7_op d7_op_id(Arena *arena, const char *s1) {
  return (d7_op){.tag = D7_ID, .v1 = d7_value_parse(arena, s1)};
}

D7_DEFINE_OP_FUNC(and, D7_AND);
D7_DEFINE_OP_FUNC(or, D7_OR);
D7_DEFINE_OP_FUNC(lshift, D7_LSHIFT);
D7_DEFINE_OP_FUNC(rshift, D7_RSHIFT);

static inline d7_op d7_op_not(Arena *arena, const char *s1) {
  return (d7_op){.tag = D7_NOT, .v1 = d7_value_parse(arena, s1)};
}

typedef struct {
  char *key;
  d7_op value;
} day07_elem;

void day07_process_line(Arena *arena, day07_elem **m, const char *line) {
  char s1[8], s2[8], s3[8];
  if (sscanf(line, "%s -> %s", s1, s2) == 2) {
    sht_put(*m, s2, d7_op_id(arena, s1));
    return;
  }
  if (sscanf(line, "NOT %s -> %s", s1, s2) == 2) {
    sht_put(*m, s2, d7_op_not(arena, s1));
    return;
  }
  if (sscanf(line, "%s AND %s -> %s", s1, s2, s3) == 3) {
    sht_put(*m, s3, d7_op_and(arena, s1, s2));
    return;
  }
  if (sscanf(line, "%s OR %s -> %s", s1, s2, s3) == 3) {
    sht_put(*m, s3, d7_op_or(arena, s1, s2));
    return;
  }
  if (sscanf(line, "%s LSHIFT %s -> %s", s1, s2, s3) == 3) {
    sht_put(*m, s3, d7_op_lshift(arena, s1, s2));
    return;
  }
  if (sscanf(line, "%s RSHIFT %s -> %s", s1, s2, s3) == 3) {
    sht_put(*m, s3, d7_op_rshift(arena, s1, s2));
    return;
  }

  fprintf(stderr, "couldn't parse line '%s'", line);
  exit(1);
}

uint16_t d7_eval_var(day07_elem *m, const char *var_name);

uint16_t d7_eval_value(day07_elem *m, d7_value val) {
  switch (val.tag) {
  case D7_INTEGER:
    return val.val.integer;
  case D7_LITERAL:
    return d7_eval_var(m, val.val.literal);
  default:
    perror("Unknown value type");
    exit(1);
  }
}

uint16_t d7_eval_op(day07_elem *m, d7_op op) {
  switch (op.tag) {
  case D7_ID:
    return d7_eval_value(m, op.v1);
  case D7_AND:
    return d7_eval_value(m, op.v1) & d7_eval_value(m, op.v2);
  case D7_OR:
    return d7_eval_value(m, op.v1) | d7_eval_value(m, op.v2);
  case D7_LSHIFT:
    return d7_eval_value(m, op.v1) << d7_eval_value(m, op.v2);
  case D7_RSHIFT:
    return d7_eval_value(m, op.v1) >> d7_eval_value(m, op.v2);
  case D7_NOT:
    return ~(d7_eval_value(m, op.v1));
  default:
    perror("Unknown op");
    exit(1);
  }
}

struct {
  char *key;
  uint16_t value;
} *d7_var_memo_g = NULL;

uint16_t d7_eval_var(day07_elem *m, const char *var_name) {
  size_t memo_idx = sht_get_idx(d7_var_memo_g, var_name);
  if (memo_idx >= 0) {
    return d7_var_memo_g[memo_idx].value;
  }
  size_t op_idx = sht_get_idx(m, var_name);
  assert(op_idx >= 0);
  uint16_t result = d7_eval_op(m, m[op_idx].value);
  sht_put(d7_var_memo_g, var_name, result);
  return result;
}

uint16_t day7(const solution_part part) {
  Arena arena = arena_create(16 * 1024);
  FILE *f = fopen("data/input07.txt", "r");
  if (f == NULL) {
    perror("error opening input file");
    return -1;
  }

  day07_elem *m = NULL;
  char line[128];
  while (fgets(line, 100, f) != NULL) {
    day07_process_line(&arena, &m, line);
  }

  printf("%lld\n", sht_size(m));

  auto result = d7_eval_var(m, "a");
  arena_free(&arena);
  sht_free(m);
  return result;
}

uint16_t day7_part1() { return day7(PART1); }
uint16_t day7_part2() { return day7(PART2); }
