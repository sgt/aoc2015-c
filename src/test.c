#include "../thirdparty/acutest.h"

#include "data_structures.h"
#include "day05.h"
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

TEST_LIST = {{"dynamic array", test_dynamic_array},
             {"test hashtable duplication", test_hashtable_duplication},
             {"test hashtable growth", test_hashtable_growth},
             {"test bitset", test_bitset},
             {"test bitset ranges", test_bitset_ranges},

             {"test day 5", test_day05},

             {NULL, NULL}};
