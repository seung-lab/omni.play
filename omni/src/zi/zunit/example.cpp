#include "zi/zunit/zunit.h"

ZUNIT_TEST(SomeTest) {
  int z = 3;
  EXPECT_EQ(3, 3);
  EXPECT_LT(1, 3);
  EXPECT_TRUE(4 > 3);
}

int main() {
  RUN_ALL_TESTS();
}
