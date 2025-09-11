#include "../thirdparty/acutest.h"

#include "data_structures.h"

void test_dynamic_array(void) {
  long *list = NULL;
  for (long i = 1; i <= 200; i++) {
    TEST_CHECK(*arr_push(&list, i) == i);
  }
  TEST_CHECK(list[123] == 124);
  TEST_CHECK(arr_pop(&list) == 200);
  TEST_CHECK(arr_pop(&list) == 199);
  TEST_CHECK(arr_last(&list) == 198);
  TEST_CHECK(arr_len(&list) == 198);
  arr_free(&list);
  TEST_CHECK(list == NULL);
}

TEST_LIST = {{"dynamic array", test_dynamic_array}, {NULL, NULL}};
