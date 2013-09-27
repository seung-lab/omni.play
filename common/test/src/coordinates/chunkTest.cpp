#include "gtest/gtest.h"

#include "common/common.h"
#include "coordinates/coordinates.h"

#include <algorithm>

using namespace om::coords;

namespace om {
namespace test {

TEST(Coords_Chunk, ParentCoord) {
  Chunk cc(1, 1, 1, 1);
  ASSERT_EQ(Chunk(2, 0, 0, 0), cc.ParentCoord());
}

TEST(Coords_Chunk, PrimarySiblingCoord) {
  Chunk cc(1, 1, 1, 1);
  ASSERT_EQ(Chunk(1, 0, 0, 0), cc.PrimarySiblingCoord());
}

TEST(Coords_Chunk, SiblingCoords) {
  Chunk cc(1, 1, 1, 1);
  std::vector<Chunk> sibs = cc.SiblingCoords();
  ASSERT_NE(sibs.end(), std::find(sibs.begin(), sibs.end(), Chunk(1, 0, 0, 0)));
  ASSERT_NE(sibs.end(), std::find(sibs.begin(), sibs.end(), Chunk(1, 0, 0, 1)));
  ASSERT_NE(sibs.end(), std::find(sibs.begin(), sibs.end(), Chunk(1, 0, 1, 0)));
  ASSERT_NE(sibs.end(), std::find(sibs.begin(), sibs.end(), Chunk(1, 1, 0, 0)));
  ASSERT_NE(sibs.end(), std::find(sibs.begin(), sibs.end(), Chunk(1, 0, 1, 1)));
  ASSERT_NE(sibs.end(), std::find(sibs.begin(), sibs.end(), Chunk(1, 1, 1, 0)));
  ASSERT_NE(sibs.end(), std::find(sibs.begin(), sibs.end(), Chunk(1, 1, 0, 1)));
  ASSERT_NE(sibs.end(), std::find(sibs.begin(), sibs.end(), Chunk(1, 1, 1, 1)));
}

TEST(Coords_Chunk, PrimaryChildCoord) {
  Chunk cc(1, 1, 1, 1);
  ASSERT_EQ(Chunk(0, 2, 2, 2), cc.PrimaryChildCoord());
}

TEST(Coords_Chunk, ChildrenCoords) {
  Chunk cc(1, 1, 1, 1);
  std::vector<Chunk> children = cc.ChildrenCoords();
  ASSERT_NE(children.end(),
            std::find(children.begin(), children.end(), Chunk(0, 2, 2, 2)));
  ASSERT_NE(children.end(),
            std::find(children.begin(), children.end(), Chunk(0, 2, 2, 3)));
  ASSERT_NE(children.end(),
            std::find(children.begin(), children.end(), Chunk(0, 2, 3, 2)));
  ASSERT_NE(children.end(),
            std::find(children.begin(), children.end(), Chunk(0, 3, 2, 2)));
  ASSERT_NE(children.end(),
            std::find(children.begin(), children.end(), Chunk(0, 2, 3, 3)));
  ASSERT_NE(children.end(),
            std::find(children.begin(), children.end(), Chunk(0, 3, 3, 2)));
  ASSERT_NE(children.end(),
            std::find(children.begin(), children.end(), Chunk(0, 3, 2, 3)));
  ASSERT_NE(children.end(),
            std::find(children.begin(), children.end(), Chunk(0, 3, 3, 3)));
}

TEST(Coords_Chunk, ToData) {
  Chunk cc(1, 1, 1, 1);
  VolumeSystem vs(Vector3i(1024));
  ASSERT_EQ(Data(128, 128, 128, vs, 1), cc.ToData(vs));
}

TEST(Coords_Chunk, BoundingBox) {
  Chunk cc(1, 1, 1, 1);
  VolumeSystem vs(Vector3i(1024));
  ASSERT_EQ(DataBbox(Data(128, 128, 128, vs, 1), Data(256, 256, 256, vs, 1)),
            cc.BoundingBox(vs));
}

TEST(Coords_Chunk, PtrOffset) {
  Chunk cc(1, 1, 1, 1);
  VolumeSystem vs(Vector3i(1024));
  size_t chunkSize = 128 * 128 * 128;
  size_t chunks = 1024 / 128 / 2;
  ASSERT_EQ(chunkSize * (chunks * chunks + chunks + 1), cc.PtrOffset(vs, 1));
  // ASSERT_EQ((128 + 1024*128 + 1024*128*128)*8, cc.PtrOffset(vs, 8));
}

TEST(Coords_Chunk, SliceDepth) {
  Chunk cc(1, 1, 1, 1);
  VolumeSystem vs(Vector3i(1024));
  ASSERT_EQ(15, cc.SliceDepth(vs, Global(266, 276, 286), common::XY_VIEW));
  ASSERT_EQ(10, cc.SliceDepth(vs, Global(266, 276, 286), common::XZ_VIEW));
  ASSERT_EQ(5, cc.SliceDepth(vs, Global(266, 276, 286), common::ZY_VIEW));
}
}
}  // namespace om::test::
