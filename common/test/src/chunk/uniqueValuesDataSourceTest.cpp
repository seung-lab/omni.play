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

#define URI "test/data/test.omni.files/segmentations/segmentation1/"

TEST(Chunk_UniqueValuesFileDataSource, Get) {
  UniqueValuesFileDataSource uvfds(URI);

  std::shared_ptr<chunk::UniqueValues> values =
      uvfds.Get(coords::Chunk(0, 0, 0, 0));
  ASSERT_TRUE((bool)values);
  ASSERT_GT(values->Values.size(), 0);

  values = uvfds.Get(coords::Chunk(100, 0, 0, 0));
  ASSERT_FALSE(values);
}

TEST(Chunk_UniqueValuesFileDataSource, Completeness) {
  volume::Segmentation s(URI);
  Voxels<uint32_t> vg(s.ChunkDS(), s.Coords());
  coords::Chunk cc(0, 1, 0, 1);

  utility::VolumeWalker<uint32_t> walker(cc.BoundingBox(s.Coords()), vg);
  std::set<uint32_t> voxels;
  walker.foreach_voxel([&](coords::Data, uint32_t val) {
    if (val) {
      voxels.insert(val);
    }
  });

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
