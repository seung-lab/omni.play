#include "gtest/gtest.h"

#include "common/common.h"
#include "coordinates/coordinates.h"

#include <algorithm>

using namespace om::coords;

namespace om {
namespace test {

TEST(Coords_Global, ToNorm) {
  VolumeSystem vs(Vector3i(1024));
  Global g(128, 256, 512);
  ASSERT_EQ(Norm(.125, .25, .5, vs), g.ToNorm(vs));
}

TEST(Coords_Global, ToData) {
  VolumeSystem vs(Vector3i(1024));
  Global g(128);
  ASSERT_EQ(Data(128, 128, 128, vs, 0), g.ToData(vs, 0));
  ASSERT_EQ(Data(64, 64, 64, vs, 1), g.ToData(vs, 1));
}

TEST(Coords_Global, ToChunk) {
  VolumeSystem vs(Vector3i(1024));
  Global g(128);
  ASSERT_EQ(Chunk(0, 1, 1, 1), g.ToChunk(vs, 0));
  Global g2(127);
  ASSERT_EQ(Chunk(0, 0, 0, 0), g2.ToChunk(vs, 0));
}

TEST(Coords_Global, ToScreen) {
  ScreenSystem ss(common::XY_VIEW, 128, 128, 1.0, Global(64));
  Global g(128);
  ASSERT_EQ(Screen(128, 128, ss), g.ToScreen(ss));
  ScreenSystem ss2(common::XY_VIEW, 128, 128, .5, Global(64));
  ASSERT_EQ(Screen(96, 96, ss2), g.ToScreen(ss2));
}

TEST(Coords_Global, WithAbsOffset) {
  VolumeSystem vs(Vector3i(1024), Vector3i(128));
  Global g(128);
  ASSERT_EQ(Global(256), g.WithAbsOffset(vs));
}

TEST(Coords_Global, FromOffsetCoords) {
  VolumeSystem vs(Vector3i(1024), Vector3i(128));
  ASSERT_EQ(Global(0), Global::FromOffsetCoords(Vector3i(128), vs));
}
}
}  // namespace om::test::
