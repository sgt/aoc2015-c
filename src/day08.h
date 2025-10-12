#pragma once

#include "common.h"

typedef struct {
  int16_t code_count, char_count, rep_count;
} d8_result;

d8_result d8_process_line(char *line) {
  if (line[0] != '"') {
    perror("line does not start with a quote");
    exit(1);
  }
  d8_result r = {.char_count = 0, .code_count = 1, .rep_count = 3};
  for (size_t i = 1; line[i] != '\0'; ++i) {
    if (line[i] == '\\') {
      switch (line[i + 1]) {
      case '\\':
      case '"':
        i += 1;
        r.code_count += 2;
        r.rep_count += 4;
        r.char_count++;
        break;
      case 'x':
        i += 3;
        r.code_count += 4;
        r.rep_count += 5;
        r.char_count++;
        break;
      default:
        fprintf(stderr, "unexpected character\n");
        exit(1);
      }
    } else if (line[i] == '"' && (line[i + 1] == '\0' || line[i + 1] == '\n')) {
      r.code_count++;
      r.rep_count += 3;
      return r;
    } else {
      r.code_count++;
      r.char_count++;
      r.rep_count++;
    }
  }
  fprintf(stderr, "unexpected eol: '%s'\n", line);
  exit(1);
}

uint16_t day8(const solution_part part) {
  FILE *f = fopen("data/input08.txt", "r");
  if (f == NULL) {
    perror("error opening input file");
    return -1;
  }

  d8_result total = {.code_count = 0, .char_count = 0, .rep_count = 0};
  char line[128];
  while (fgets(line, 100, f) != NULL) {
    d8_result cur = d8_process_line(line);
    total.code_count += cur.code_count;
    total.char_count += cur.char_count;
    total.rep_count += cur.rep_count;
  }

  fclose(f);

  return part == PART1 ? total.code_count - total.char_count
                       : total.rep_count - total.code_count;
}

uint16_t day8_part1() { return day8(PART1); }
uint16_t day8_part2() { return day8(PART2); }
