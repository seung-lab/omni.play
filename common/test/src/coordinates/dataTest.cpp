#include "gtest/gtest.h"

#include "common/common.h"
#include "coordinates/coordinates.h"

#include <algorithm>

using namespace om::coords;

namespace om {
namespace test {

TEST(Coords_Data, ToGlobal) {
  VolumeSystem vs(Vector3i(1024));
  Data d(128, 128, 128, vs, 0);
  ASSERT_EQ(Global(128), d.ToGlobal());
}

TEST(Coords_Data, ToNorm) {
  VolumeSystem vs(Vector3i(1024));
  Data d(128, 128, 128, vs, 0);
  ASSERT_EQ(Norm(.125, .125, .125, vs), d.ToNorm());
}

TEST(Coords_Data, ToChunk) {
  VolumeSystem vs(Vector3i(1024));
  Data d(128, 128, 128, vs, 0);
  ASSERT_EQ(Chunk(0, 1, 1, 1), d.ToChunk());
}

TEST(Coords_Data, ToChunkVec) {
  VolumeSystem vs(Vector3i(1024));
  Data d(128, 128, 128, vs, 0);
  ASSERT_EQ(Vector3i(0), d.ToChunkVec());
}

TEST(Coords_Data, ToChunkOffset) {
  VolumeSystem vs(Vector3i(1024));
  Data d(129, 129, 129, vs, 0);
  ASSERT_EQ(128 * 128 + 128 + 1, d.ToChunkOffset());
}

TEST(Coords_Data, ToTileOffset) {
  VolumeSystem vs(Vector3i(1024));
  Data d(129, 129, 129, vs, 0);
  ASSERT_EQ(129, d.ToTileOffset(common::XY_VIEW));
}

TEST(Coords_Data, ToTileDepth) {
  VolumeSystem vs(Vector3i(1024));
  Data d(129, 129, 129, vs, 0);
  ASSERT_EQ(1, d.ToTileDepth(common::XY_VIEW));
}

TEST(Coords_Data, IsInVolume) {
  VolumeSystem vs(Vector3i(1024));
  Data d(128, 128, 128, vs, 0);
  ASSERT_TRUE(d.IsInVolume());
  Data d2(12800, 128, 128, vs, 0);
  ASSERT_FALSE(d2.IsInVolume());
}
}
}  // namespace om::test::
