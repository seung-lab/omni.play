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

#define URI "../../test_data/x06y59z28_s1587_13491_6483_e1842_13746_6738.omni"

TEST(Chunk_UniqueValuesFileDataSource, Get) {
  file::Paths p(URI);
  UniqueValuesFileDataSource uvfds(p.Segmentation(1));

  std::shared_ptr<chunk::UniqueValues> values =
      uvfds.Get(coords::Chunk(0, 0, 0, 0));
  ASSERT_TRUE((bool)values);
  ASSERT_GT(values->Values.size(), 0);

  //Becase this doesn't exists a null pointer is returned
  values = uvfds.Get(coords::Chunk(100, 0, 0, 0));
  ASSERT_FALSE((bool)values);
}

TEST(Chunk_UniqueValuesFileDataSource, Completeness) {
  volume::Segmentation s(file::Paths(URI), 1);
  coords::Chunk cc(0, 1, 1, 1);

  common::SegIDSet voxels;
  auto iterable = s.Iterate<common::SegID>(cc.BoundingBox(s.Coords()));
  for (auto& iter : iterable) {
    if (iter.value()) {
      voxels.insert(iter.value());
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
