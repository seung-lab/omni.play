#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "utility/UUID.hpp"
#include "chunk/uniqueValuesFileDataSource.hpp"
#include "chunk/dataSources.hpp"
#include "chunk/voxelGetter.hpp"
#include "utility/volumeWalker.hpp"
#include "volume/segmentation.h"

using namespace om::chunk;

namespace om {
namespace test {

#define URI "/omniData/e2198/e2198_a_s10_101_46_e17_116_61.omni"

TEST(Chunk_UniqueValuesFileDataSource, Get) {
  file::Paths p(URI);
  UniqueValuesFileDataSource uvfds(p.Segmentation(1));

  std::shared_ptr<chunk::UniqueValues> values =
      uvfds.Get(coords::Chunk(0, 0, 0, 0));
  ASSERT_TRUE((bool)values);
  ASSERT_GT(values->Values.size(), 0);

  values = uvfds.Get(coords::Chunk(100, 0, 0, 0));
  ASSERT_FALSE(values);
}

TEST(Chunk_UniqueValuesFileDataSource, Completeness) {
  volume::Segmentation s(file::Paths(URI), 1);
  coords::Chunk cc(0, 1, 1, 1);

  common::SegIDSet voxels;
  auto iterable = s.Iterate<common::SegID>(cc.BoundingBox(s.Coords()));
  for (auto& iter : iterable) {
    if (iter.second) {
      voxels.insert(iter.second);
    }
  }
  auto uv = s.UniqueValuesDS().Get(cc);

  ASSERT_EQ(voxels.size(), uv->Values.size());

  for (auto& v : voxels) {
    ASSERT_TRUE(uv->contains(v));
  }
}

TEST(Chunk_UniqueValuesFileDataSource, Put) {}

#undef URI
}
}  // namespace om::test::
