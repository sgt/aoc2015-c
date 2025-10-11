#pragma once

#include "arena.h"
#include "common.h"
#include "data_structures.h"
#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
  enum { D7_LITERAL, D7_INTEGER } tag;
  union {
    char *literal;
    uint16_t integer;
  } val;
} d7_value;

typedef struct {
  enum { D7_ID, D7_AND, D7_OR, D7_LSHIFT, D7_RSHIFT, D7_NOT } tag;
  d7_value v1, v2;
} d7_op;

typedef struct {
  char *key;
  d7_op value;
} d7_signal;

typedef struct {
  Arena arena; // for storing strings
  struct {
    char *key;
    d7_op value;
  } *signals; // for storing signal definitions
  struct {
    char *key;
    uint16_t value;
  } *var_cache; // for caching computed values
} d7_machine;

d7_machine d7_machine_create(size_t arena_size) {
  return (d7_machine){
      .arena = arena_create(arena_size),
      .signals = NULL,
      .var_cache = NULL,
  };
}

void d7_machine_free(d7_machine *m) {
  sht_free(m->signals);
  sht_free(m->var_cache);
  arena_free(&m->arena);
}

bool _isalnumstr(const char *s) {
  assert(s != NULL);
  for (size_t i = 0; s[i] != '\0'; ++i) {
    if (!isalnum(s[i])) {
      return false;
    }
  }
  return true;
}

static inline d7_value d7_value_literal(d7_machine *m, const char *s) {
  char *s_copy = arena_strdup(&m->arena, s);
  assert(s_copy != NULL);
  return (d7_value){.tag = D7_LITERAL, .val = {.literal = s_copy}};
}

static inline d7_value d7_value_int(uint16_t n) {
  return (d7_value){.tag = D7_INTEGER, .val = {.integer = n}};
}

static inline d7_value d7_value_parse(d7_machine *m, const char *s) {
  return isdigit(s[0]) ? d7_value_int(atoi(s)) : d7_value_literal(m, s);
}

#define D7_DEFINE_OP_FUNC(Name, Tag)                                           \
  static inline d7_op d7_op_##Name(d7_machine *m, const char *s1,              \
                                   const char *s2) {                           \
    return (d7_op){                                                            \
        .tag = Tag, .v1 = d7_value_parse(m, s1), .v2 = d7_value_parse(m, s2)}; \
  }

static inline d7_op d7_op_id(d7_machine *m, const char *s1) {
  return (d7_op){.tag = D7_ID, .v1 = d7_value_parse(m, s1)};
}

D7_DEFINE_OP_FUNC(and, D7_AND);
D7_DEFINE_OP_FUNC(or, D7_OR);
D7_DEFINE_OP_FUNC(lshift, D7_LSHIFT);
D7_DEFINE_OP_FUNC(rshift, D7_RSHIFT);

static inline d7_op d7_op_not(d7_machine *m, const char *s1) {
  return (d7_op){.tag = D7_NOT, .v1 = d7_value_parse(m, s1)};
}

void day07_process_line(d7_machine *m, const char *line) {
  char s1[8], s2[8], s3[8];
  if (sscanf(line, "%s -> %s", s1, s2) == 2) {
    sht_put(m->signals, s2, d7_op_id(m, s1));
    return;
  }
  if (sscanf(line, "NOT %s -> %s", s1, s2) == 2) {
    sht_put(m->signals, s2, d7_op_not(m, s1));
    return;
  }
  if (sscanf(line, "%s AND %s -> %s", s1, s2, s3) == 3) {
    sht_put(m->signals, s3, d7_op_and(m, s1, s2));
    return;
  }
  if (sscanf(line, "%s OR %s -> %s", s1, s2, s3) == 3) {
    sht_put(m->signals, s3, d7_op_or(m, s1, s2));
    return;
  }
  if (sscanf(line, "%s LSHIFT %s -> %s", s1, s2, s3) == 3) {
    sht_put(m->signals, s3, d7_op_lshift(m, s1, s2));
    return;
  }
  if (sscanf(line, "%s RSHIFT %s -> %s", s1, s2, s3) == 3) {
    sht_put(m->signals, s3, d7_op_rshift(m, s1, s2));
    return;
  }

  fprintf(stderr, "couldn't parse line '%s'", line);
  exit(1);
}

uint16_t d7_eval_var(d7_machine *m, const char *var_name);

uint16_t d7_eval_value(d7_machine *m, d7_value val) {
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

uint16_t d7_eval_op(d7_machine *m, d7_op op) {
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

uint16_t d7_eval_var(d7_machine *m, const char *var_name) {
  ptrdiff_t memo_idx = sht_get_idx(m->var_cache, var_name);
  if (memo_idx >= 0) {
    return m->var_cache[memo_idx].value;
  }
  ptrdiff_t op_idx = sht_get_idx(m->signals, var_name);
  assert(op_idx >= 0);
  uint16_t result = d7_eval_op(m, m->signals[op_idx].value);
  sht_put(m->var_cache, var_name, result);
  return result;
}

uint16_t day7(const solution_part part) {
  FILE *f = fopen("data/input07.txt", "r");
  if (f == NULL) {
    perror("error opening input file");
    return -1;
  }

  d7_machine m = d7_machine_create(16 * 1024);

  char line[128];
  while (fgets(line, 100, f) != NULL) {
    day07_process_line(&m, line);
  }

  uint16_t result_a = d7_eval_var(&m, "a");
  if (part == PART2) {
    d7_op b_val = (d7_op){.tag = D7_ID, .v1 = d7_value_int(result_a)};
    sht_put(m.signals, "b", b_val);
    sht_free(m.var_cache);
    result_a = d7_eval_var(&m, "a");
  }

  d7_machine_free(&m);
  return result_a;
}

uint16_t day7_part1() { return day7(PART1); }
uint16_t day7_part2() { return day7(PART2); }
