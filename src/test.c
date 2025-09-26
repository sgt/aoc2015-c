#include "../thirdparty/acutest.h"

#include "arena.h"
#include "data_structures.h"
#include "day05.h"
#include "day06.h"
#include "day07.h"
#include <stdint.h>

#define TEST_DYN_ARRAY(type)                                                   \
  {                                                                            \
    type *list = NULL;                                                         \
    for (type i = 1; i <= 200; ++i) {                                          \
      arr_push(list, i);                                                       \
    }                                                                          \
    TEST_CHECK(list[123] == 124);                                              \
    TEST_CHECK(arr_pop(list) == 200);                                          \
    TEST_CHECK(arr_pop(list) == 199);                                          \
    TEST_CHECK(arr_last(list) == 198);                                         \
    TEST_CHECK(arr_len(list) == 198);                                          \
    arr_free(list);                                                            \
    TEST_CHECK(list == NULL);                                                  \
  }

void test_dynamic_array(void) {
  TEST_DYN_ARRAY(float);
  TEST_DYN_ARRAY(uint32_t)

  typedef struct {
    uint32_t x, y;
  } pair;
  pair *list = NULL;
  for (uint32_t i = 1; i <= 200; ++i) {
    arr_push(list, ((pair){.x = i, .y = i * 2}));
  }
  TEST_CHECK(list[123].x == 124 && list[123].y == 248);
  pair p = arr_pop(list);
  TEST_CHECK(p.x == 200 && p.y == 400);
  arr_free(list);
  TEST_CHECK(list == NULL);
}

typedef struct {
  int x, y;
} pair;

void test_hashtable_duplication(void) {
  struct {
    pair key;
    int value;
  } *m = NULL;
  pair k = {0, 0};
  pair k2 = {10, 20};

  ht_put(m, k, 1);
  TEST_CHECK(ht_size(m) == 1);
  TEST_CHECK(ht_get(m, k) == 1);
  ht_put(m, k, 2);
  TEST_CHECK(ht_size(m) == 1);
  TEST_CHECK(ht_get(m, k) == 2);
  ht_put(m, k2, 3);
  TEST_CHECK(ht_size(m) == 2);
  TEST_CHECK(ht_get(m, k) == 2);
  TEST_CHECK(ht_get(m, k2) == 3);
  ht_free(m);
  TEST_CHECK(m == NULL);
}

void test_hashtable_growth(void) {
  struct {
    pair key;
    int value;
  } *m = NULL;

  TEST_CHECK(!ht_has(m, ((pair){0, 0})));
  for (int i = 1; i <= 200; ++i) {
    pair key = (pair){i, i * 2};
    int val = i * 3;
    ht_put(m, key, val);
  }
  TEST_CHECK(ht_size(m) == 200);
  TEST_CHECK(ht_has(m, ((pair){20, 40})));
  TEST_CHECK(!ht_has(m, ((pair){20, 4})));
  TEST_CHECK(ht_get(m, ((pair){30, 60})) == 90);
  ht_free(m);
  TEST_CHECK(m == NULL);
}

void test_string_hashtable_duplication(void) {
  struct {
    char *key;
    int value;
  } *m = NULL;

  char *k = "foo";
  char *k2 = "boulbasaurus";

  sht_put(m, k, 1);
  TEST_CHECK(sht_size(m) == 1);
  TEST_CHECK(sht_get(m, k) == 1);
  TEST_CHECK(sht_get_or(m, k, -1) == 1);
  TEST_CHECK(sht_get_or(m, k2, -1) == -1);
  sht_put(m, k, 2);
  TEST_CHECK(sht_size(m) == 1);
  TEST_CHECK(sht_get(m, k) == 2);
  sht_put(m, k2, 3);
  TEST_CHECK(sht_size(m) == 2);
  TEST_CHECK(sht_get(m, k) == 2);
  TEST_CHECK(sht_get(m, k2) == 3);
  sht_free(m);
  TEST_CHECK(m == NULL);
}

void test_string_hashtable_growth(void) {
  struct {
    char *key;
    int value;
  } *m = NULL;

  TEST_CHECK(!sht_has(m, "foo"));
  for (int i = 1; i <= 200; ++i) {
    char key[16];
    sprintf(key, "meow%d", i);
    int val = i;
    sht_put(m, key, val);
  }
  TEST_CHECK(sht_size(m) == 200);
  TEST_CHECK(sht_has(m, "meow20"));
  TEST_CHECK(!sht_has(m, "meow300"));
  TEST_CHECK(sht_get(m, "meow42") == 42);
  sht_free(m);
  TEST_CHECK(m == NULL);
}

void test_bitset(void) {
  bitset *bs = bitset_create(5);

  // getting value beyond capacity
  TEST_CHECK(!bitset_get(bs, 12));

  // setting
  for (int i = 1; i < 20; ++i) {
    bitset_set(bs, i);
    for (int j = 1; j < 20; ++j) {
      TEST_CHECK(bitset_get(bs, j) == (j <= i));
    }
    TEST_CHECK(bitset_cardinality(bs) == i);
  }

  // setting and growing
  bitset_set(bs, 100);
  TEST_CHECK(bitset_get(bs, 100));
  TEST_CHECK(!bitset_get(bs, 0));
  TEST_CHECK(!bitset_get(bs, 99));
  TEST_CHECK(!bitset_get(bs, 199));
  TEST_CHECK(bitset_cardinality(bs) == 20);

  // flipping
  bitset_flip(bs, 200);
  TEST_CHECK(bitset_get(bs, 200));
  TEST_CHECK(bitset_cardinality(bs) == 21);
  bitset_flip(bs, 200);
  TEST_CHECK(!bitset_get(bs, 200));
  TEST_CHECK(bitset_cardinality(bs) == 20);

  // clearing
  bitset_clear(bs, 100);
  TEST_CHECK(!bitset_get(bs, 100));
  TEST_CHECK(bitset_cardinality(bs) == 19);

  bitset_free(bs);
}

void test_bitset_ranges(void) {
  bitset *bs = bitset_create(5);

  {
    // set bits 5,..20
    bitset_range_set(bs, 5, 16);
    TEST_CHECK(bitset_cardinality(bs) == 16);
    TEST_MSG("cardinality: %zu\n", bitset_cardinality(bs));
    for (size_t i = 0; i < 24; i++) {
      TEST_CHECK(bitset_get(bs, i) == (i >= 5 && i <= 20));
      TEST_MSG("i: %zu bit: %d\n", i, bitset_get(bs, i));
    }
  }

  {
    // clear bits 6..18
    bitset_range_clear(bs, 6, 13);
    TEST_CHECK(bitset_cardinality(bs) == 3);
    TEST_MSG("cardinality: %zu\n", bitset_cardinality(bs));
    for (size_t i = 0; i < 24; i++) {
      TEST_CHECK(bitset_get(bs, i) == (i == 5 || (i >= 19 && i <= 20)));
      TEST_MSG("i: %zu bit: %d\n", i, bitset_get(bs, i));
    }
  }

  bitset_free(bs);

  // flip
  bs = bitset_create(16);
  // set bits 5..12
  bitset_range_set(bs, 5, 8);
  TEST_CHECK(bitset_cardinality(bs) == 8);
  // flip bits 3..10
  bitset_range_flip(bs, 3, 8);
  // bits set now are 3..4, 11..12
  TEST_CHECK(bitset_cardinality(bs) == 4);
  for (size_t i = 0; i < 16; i++) {
    TEST_CHECK(bitset_get(bs, i) == (i == 3 || i == 4 || i == 11 || i == 12));
    TEST_MSG("i: %zu bit: %d\n", i, bitset_get(bs, i));
  }
  bitset_free(bs);
}

void test_arena(void) {
  Arena arena = arena_create(8);
  TEST_CHECK(arena.capacity == 8);
  TEST_CHECK(arena.offset == 0);
  uint32_t *n = arena_alloc(&arena, sizeof(uint32_t), sizeof(uint32_t));
  *n = 42;
  TEST_CHECK(arena.capacity == 8);
  TEST_CHECK(arena.offset == 4);
  char *foo = arena_strdup(&arena, "meow");
  TEST_CHECK(strcmp(foo, "meow") == 0);
  TEST_CHECK(arena.capacity == 16);
  TEST_CHECK(arena.offset == 9);
  uint64_t *n2 = arena_alloc(&arena, sizeof(uint64_t), sizeof(uint64_t));
  *n2 = 12345678;
  TEST_CHECK(arena.capacity == 32);
  TEST_CHECK(arena.offset == 24);
  arena_reset(&arena);
  TEST_CHECK(arena.capacity == 32);
  TEST_CHECK(arena.offset == 0);
  arena_free(&arena);
  TEST_CHECK(arena.data == NULL);
}

void test_day05(void) {
  TEST_CHECK(is_nice("ugknbfddgicrmopn"));
  TEST_CHECK(is_nice("aaa"));
  TEST_CHECK(!is_nice("jchzalrnumimnmhp"));
  TEST_CHECK(!is_nice("haegwjzuvuyypxyu"));
  TEST_CHECK(!is_nice("dvszwmarrgswjxmb"));

  TEST_CHECK(is_nice2("qjhvhtzxzqqjkmpb"));
  TEST_CHECK(is_nice2("xxyxx"));
  TEST_CHECK(!is_nice2("uurcxstgmygtbstg"));
  TEST_CHECK(!is_nice2("ieodomkazucvgmuy"));
  TEST_CHECK(!is_nice2("aaa"));
  TEST_CHECK(is_nice2("aaaa"));
}

void test_day06(void) {
  {
    uint8_t arr[1000 * 1000] = {0};
    day06_perform_cmd(arr, PART1, "turn on", 0, 0, 999, 999);
    TEST_CHECK(day06_total(arr) == 1000 * 1000);
  }
}

void test_day07(void) {
  Arena arena = arena_create(256);
  day07_elem *m = NULL;
  day07_process_line(&arena, &m, "123 -> x");
  day07_process_line(&arena, &m, "456 -> y");
  day07_process_line(&arena, &m, "x AND y -> d");
  day07_process_line(&arena, &m, "x OR y -> e");
  day07_process_line(&arena, &m, "x LSHIFT 2 -> f");
  day07_process_line(&arena, &m, "y RSHIFT 2 -> g");
  day07_process_line(&arena, &m, "NOT x -> h");
  day07_process_line(&arena, &m, "NOT y -> i");
  TEST_CHECK(d7_eval_var(m, "d") == 72);
  TEST_CHECK(d7_eval_var(m, "e") == 507);
  TEST_CHECK(d7_eval_var(m, "f") == 492);
  TEST_CHECK(d7_eval_var(m, "g") == 114);
  TEST_CHECK(d7_eval_var(m, "h") == 65412);
  TEST_CHECK(d7_eval_var(m, "i") == 65079);
  TEST_CHECK(d7_eval_var(m, "x") == 123);
  TEST_CHECK(d7_eval_var(m, "y") == 456);
  sht_free(m);
  arena_free(&arena);
}

TEST_LIST = {
    {"dynamic array", test_dynamic_array},
    {"test hashtable duplication", test_hashtable_duplication},
    {"test hashtable growth", test_hashtable_growth},
    {"test string hashtable duplication", test_string_hashtable_duplication},
    {"test string hashtable growth", test_string_hashtable_growth},
    {"test bitset", test_bitset},
    {"test bitset ranges", test_bitset_ranges},

    {"test arena", test_arena},

    {"test day 5", test_day05},
    {"test day 6", test_day06},
    {"test day 7", test_day07},

    {NULL, NULL}};
