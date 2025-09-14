
#include <stdbool.h>
#include <stdio.h>

#include "common.h"
#include "day01.h"
#include "day02.h"

int main(const int argc, const char *argv[]) {
  if (argc != 2) {
    puts("specify problem number");
    return 1;
  }

  uint32_t n;
  bool ok = str2uint32(argv[1], &n);
  if (!ok) {
    puts("argument must be a number");
    return 1;
  }

  switch (n) {
  case 1:
    printf("1.1: %zu\n", day1_part1());
    printf("1.2: %zu\n", day1_part2());
    break;
  case 2:
    printf("2.1: %d\n", day2_part1());
    printf("2.2: %d\n", day2_part2());
    break;
  default:
    printf("there is no problem %d\n", n);
  }

  return 0;
}
