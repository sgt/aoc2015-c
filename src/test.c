#include "../thirdparty/acutest.h"

#include "data_structures.h"
#include <stdint.h>

#define TEST_DYN_ARRAY(type)                                                   \
  {                                                                            \
    type *list = NULL;                                                         \
    for (type i = 1; i <= 200; i++) {                                          \
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
  for (uint32_t i = 1; i <= 200; i++) {
    arr_push(list, ((pair){.x = i, .y = i * 2}));
  }
  TEST_CHECK(list[123].x == 124 && list[123].y == 248);
  pair p = arr_pop(list);
  TEST_CHECK(p.x == 200 && p.y == 400);
  arr_free(list);
  TEST_CHECK(list == NULL);
}

void test_hashtable(void) {
  typedef struct {
    int x, y;
  } pair;
  struct {
    pair key;
    int value;
  } *m = NULL;
  for (int i = 1; i <= 200; i++) {
    ht_put(m, ((pair){i, i * 2}), i * 3);
  }
  TEST_CHECK(ht_size(m) == 200);
  TEST_CHECK(ht_has(m, ((pair){20, 40})));
  TEST_CHECK(!ht_has(m, ((pair){20, 4})));
  TEST_CHECK(ht_get(m, ((pair){30, 60})) == 90);
  ht_free(m);
  TEST_CHECK(m == NULL);
}

TEST_LIST = {{"dynamic array", test_dynamic_array},
             {"test hashtable", test_hashtable},
             {NULL, NULL}};
