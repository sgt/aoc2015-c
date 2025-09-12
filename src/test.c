#include "../thirdparty/acutest.h"

#include "data_structures.h"

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
  TEST_DYN_ARRAY(long)

  typedef struct {
    long x, y;
  } pair;
  pair *list = NULL;
  for (long i = 1; i <= 200; i++) {
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
} vec;

void test_hashtable(void) {
  struct {
    vec key;
    int value;
  } *m = NULL;
}

TEST_LIST = {{"dynamic array", test_dynamic_array}, {NULL, NULL}};
