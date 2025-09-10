#include "../thirdparty/acutest.h"

void test_foo(void) {
    TEST_CHECK(2 + 3 == 5);
}

TEST_LIST = {
    {"foo", test_foo},
    {NULL, NULL}
};
