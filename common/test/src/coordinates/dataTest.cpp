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

TEST(Coords_Data, AtDifferentLevel) {
  VolumeSystem vs(Vector3i(2048));
  Data d(127, 126, 125, vs, 1);
  ASSERT_EQ(Vector3i(254,252,250), d.AtDifferentLevel(0));
  ASSERT_EQ(d, d.AtDifferentLevel(1));
  ASSERT_EQ(Vector3i(63,63,62), d.AtDifferentLevel(2));
  ASSERT_EQ(Vector3i(31,31,31), d.AtDifferentLevel(3));
  ASSERT_EQ(Vector3i(15,15,15), d.AtDifferentLevel(4));
}
}
}  // namespace om::test::
