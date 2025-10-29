#pragma once

#include "../thirdparty/json.h"
#include "common.h"
#include <assert.h>
#include <ctype.h>
#include <string.h>

typedef struct {
  const char *input;
  usize idx;
} d12_num_finder;

d12_num_finder d12_init_num_finder(const char *s) {
  return (d12_num_finder){.input = s, .idx = 0};
}

bool d12_isnumchar(int c) { return c == '-' || isdigit(c); }

bool d12_next_num(d12_num_finder *finder, int *num) {
  while (!d12_isnumchar(finder->input[finder->idx]) &&
         finder->input[finder->idx] != '\0') {
    ++finder->idx;
  }
  if (finder->input[finder->idx] == '\0')
    return false;
  int start = finder->idx;
  do {
    ++finder->idx;
  } while (d12_isnumchar(finder->input[finder->idx]));
  int len = finder->idx - start;
  assert(len < 128);
  char buf[128];
  memcpy(buf, &finder->input[start], len);
  buf[len] = '\0';
  *num = atoi(buf);
  return true;
}

bool d12_is_red(const struct json_object_s *obj) {
  for (struct json_object_element_s *e = obj->start; e != NULL; e = e->next) {
    struct json_string_s *val = json_value_as_string(e->value);
    if (val != NULL && strcmp(val->string, "red") == 0) {
      return true;
    }
  }
  return false;
}

int d12_sum_arr(const struct json_array_s *);
int d12_sum_obj(const struct json_object_s *);

int d12_sum_val(struct json_value_s *val) {
  switch (val->type) {
  case json_type_object:
    return d12_sum_obj(json_value_as_object(val));
  case json_type_array:
    return d12_sum_arr(json_value_as_array(val));
  case json_type_number:
    return atoi(json_value_as_number(val)->number);
  default: // do nothing
    return 0;
  }
}

int d12_sum_arr(const struct json_array_s *arr) {
  int sum = 0;
  for (struct json_array_element_s *e = arr->start; e != NULL; e = e->next) {
    sum += d12_sum_val(e->value);
  }
  return sum;
}

int d12_sum_obj(const struct json_object_s *obj) {
  if (d12_is_red(obj))
    return 0;

  int sum = 0;
  for (struct json_object_element_s *e = obj->start; e != NULL; e = e->next) {
    sum += d12_sum_val(e->value);
  }
  return sum;
}

int d12_sum_non_red(const char *s) {
  struct json_value_s *root = json_parse(s, strlen(s));
  return d12_sum_obj((struct json_object_s *)root->payload);
}

int day12(const solution_part part) {

  FILE *f = fopen("data/input12.txt", "r");
  if (f == NULL) {
    perror("error opening input file");
    return -1;
  }

  char line[1024 * 100];
  if (fgets(line, 1024 * 100, f) == NULL) {
    perror("error reading file");
    return -1;
  }
  fclose(f);

  int sum = 0;

  switch (part) {
  case PART1:
    // using custom parser
    d12_num_finder finder = d12_init_num_finder(line);
    int n;
    while (d12_next_num(&finder, &n)) {
      sum += n;
    }
    break;
  case PART2:
    // using third-party json parser
    return d12_sum_non_red(line);
    break;
  }

  return sum;
}

int day12_part1() { return day12(PART1); }
int day12_part2() { return day12(PART2); }
