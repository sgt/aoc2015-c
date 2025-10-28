
#include <stdbool.h>
#include <stdio.h>

#include "../thirdparty/md5.c"
#include "common.h"
#include "day01.h"
#include "day02.h"
#include "day03.h"
#include "day04.h"
#include "day05.h"
#include "day06.h"
#include "day07.h"
#include "day08.h"
#include "day09.h"
#include "day10.h"
#include "day11.h"
#include "day12.h"

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
  case 3:
    printf("3.1: %d\n", day3_part1());
    printf("3.2: %d\n", day3_part2());
    break;
  case 4:
    printf("4.1: %d\n", day4_part1());
    printf("4.2: %d\n", day4_part2());
    break;
  case 5:
    printf("5.1: %d\n", day5_part1());
    printf("5.2: %d\n", day5_part2());
    break;
  case 6:
    printf("6.1: %d\n", day6_part1());
    printf("6.2: %d\n", day6_part2());
    break;
  case 7:
    printf("7.1: %d\n", day7_part1());
    printf("7.2: %d\n", day7_part2());
    break;
  case 8:
    printf("8.1: %d\n", day8_part1());
    printf("8.2: %d\n", day8_part2());
    break;
  case 9:
    printf("9.1: %d\n", day9_part1());
    printf("9.2: %d\n", day9_part2());
    break;
  case 10:
    printf("10.1: %zu\n", day10_part1());
    printf("10.2: %zu\n", day10_part2());
    break;
  case 11:
    char *d11_1 = day11_part1();
    printf("11.1: %s\n", day11_part1());
    free(d11_1);
    char *d11_2 = day11_part2();
    printf("11.2: %s\n", day11_part2());
    free(d11_2);
    break;
  case 12:
    printf("12.1: %d\n", day12_part1());
    printf("12.2: %d\n", day12_part2());
    break;
  default:
    printf("there is no problem #%d\n", n);
  }

  return 0;
}
