#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "utility/iterators.hpp"

namespace om {
namespace test {

using namespace utility;

TEST(Utility_Iterators, VectorIterator) {
  size_t count = 0;

  for (auto iter = vector3_iterator<int>({1, 2, 3}, {2, 4, 6});
       iter != vector3_iterator<int>(); ++iter) {
    count++;
    if (count > 30) {
      break;
    }
  }
  ASSERT_EQ(24, count);
  Vector3i val(1, 4, 3);
  ASSERT_NE(vector3_iterator<int>(),
            std::find(vector3_iterator<int>({1, 2, 3}, {2, 4, 6}),
                      vector3_iterator<int>(), val));
}
}
}  // namespace om::test::