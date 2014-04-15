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

TEST(Utility_Iterators, ChunkIterator) {
  size_t count = 0;

  for (auto iter = make_chunk_iterator(1, {1, 2, 3}, {2, 4, 6});
       iter != make_chunk_iterator(); ++iter) {
    count++;
    if (count > 30) {
      break;
    }
  }
  ASSERT_EQ(24, count);
  coords::Chunk val(1, 1, 4, 3);
  ASSERT_NE(make_chunk_iterator(),
            std::find(make_chunk_iterator(1, {1, 2, 3}, {2, 4, 6}),
                      make_chunk_iterator(), val));
}
}
}  // namespace om::test::