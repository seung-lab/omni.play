#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "utility/UUID.hpp"
#include "chunk/uniqueValuesFileDataSource.hpp"
#include "chunk/dataSources.hpp"

using namespace om::chunk;

namespace om {
namespace test {

#define URI "test/data/test.omni.files/segmentations/segmentation1/"

TEST(Chunk_UniqueValuesFileDataSource, Get) {
  UniqueValuesFileDataSource uvfds(URI);

  std::shared_ptr<chunk::UniqueValues> values =
      uvfds.Get(coords::Chunk(0, 0, 0, 0));
  ASSERT_TRUE((bool) values);
  ASSERT_GT(values->Values.size(), 0);

  values = uvfds.Get(coords::Chunk(100, 0, 0, 0));
  ASSERT_FALSE(values);
}

TEST(Chunk_UniqueValuesFileDataSource, Put) {}

#undef URI
}
}  // namespace om::test::
