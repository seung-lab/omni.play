#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "utility/iterators.hpp"
#include "utility/timer.hpp"

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

  vector3_iterator<int> a({1, 2, 3}, {1, 2, 3});
  vector3_iterator<int> b;
  ASSERT_NE(b, a);
  ASSERT_EQ(b, ++a);
  ASSERT_NE(vector3_iterator<int>({0, 0, 0}, {10, 11, 12}),
            vector3_iterator<int>());
}

TEST(Utility_Iterators, Benchmark_VectorIterator) {
  const size_t LIMIT = 500;
  utility::timer t;
  t.start();
  size_t inc = 0;
  for (int i = 1; i <= LIMIT; ++i) {
    for (int j = 1; j <= LIMIT; ++j) {
      for (int k = 1; k <= LIMIT; ++k) {
        if ((j + k) % i == 0) inc++;
      }
    }
  }
  t.Print("Standard For loop");

  t.reset();
  t.start();
  size_t inc2 = 0;
  for (vector3_iterator<int> iter({1, 1, 1}, {LIMIT, LIMIT, LIMIT});
       iter != vector3_iterator<int>(); ++iter) {
    auto ref = *iter;
    if ((ref.z + ref.y) % ref.x == 0) inc2++;
  }
  t.Print("vector3_iterator");

  ASSERT_EQ(inc, inc2);
}

TEST(Utility_Iterators, Benchmark_ChunkIterator) {
  const size_t LIMIT = 200;
  coords::VolumeSystem vs(Vector3i{LIMIT, LIMIT, LIMIT});

  utility::timer t;
  t.start();
  size_t val;
  for (int i = 0; i <= LIMIT; ++i) {
    for (int j = 0; j <= LIMIT; ++j) {
      for (int k = 0; k <= LIMIT; ++k) {
        coords::Chunk c(0, i, j, k);
        val = (val + c.PtrOffset(vs, 32)) % LIMIT;
      }
    }
  }
  t.Print("Standard For loop");

  t.reset();
  t.start();
  size_t val2 = 0;
  for (auto iter = make_chunk_iterator(coords::Chunk(0, 0, 0, 0),
                                       coords::Chunk(0, LIMIT, LIMIT, LIMIT));
       iter != chunk_iterator(); ++iter) {
    val2 = (val2 + iter->PtrOffset(vs, 32)) % LIMIT;
  }
  t.Print("chunk_iterator");

  ASSERT_EQ(val, val2);
}

// TEST(Utility_Iterators, ChunkIterator) {
//   size_t count = 0;

//   for (auto iter = make_chunk_iterator(1, {1, 2, 3}, {2, 4, 6});
//        iter != make_chunk_iterator(); ++iter) {
//     count++;
//     if (count > 30) {
//       break;
//     }
//   }
//   ASSERT_EQ(24, count);
//   coords::Chunk val(1, 1, 4, 3);
//   ASSERT_NE(make_chunk_iterator(),
//             std::find(make_chunk_iterator(1, {1, 2, 3}, {2, 4, 6}),
//                       make_chunk_iterator(), val));
// }
}
}  // namespace om::test::