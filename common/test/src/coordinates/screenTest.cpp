#include "gtest/gtest.h"

#include "common/common.h"
#include "coordinates/coordinates.h"

#include <algorithm>

using namespace om::coords;

namespace om {
namespace test {

// xg = (xs - w/2) / scale + lx
// yg = (ys - h/2) / scale + ly
// zg = lz
TEST(Coords_Screen, ToGlobal) {
  ScreenSystem ss(common::XY_VIEW, 128, 128, 1.0, Global(64));
  Screen s(128, 128, ss);
  ASSERT_EQ(Global(128, 128, 64), s.ToGlobal());
}

// xg = (xs - w/2) / scale + lx
// yg = (ys - h/2) / scale + ly
// zg = lz
TEST(Coords_Screen, ToGlobal_Complex) {
  ScreenSystem ss(common::XZ_VIEW, 128, 64, 2.0, Global(64));
  Screen s(10, 10, ss);
  ASSERT_EQ(Global(37, 64, 53), s.ToGlobal());
  ScreenSystem ss2(common::ZY_VIEW, 64, 128, .5, Global(128));
  Screen s2(10, 10, ss2);
  ASSERT_EQ(Global(128, 20, 84), s2.ToGlobal());
}
}
}  // namespace om::test::
