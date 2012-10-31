#include "gtest/gtest.h"

#include "common/common.h"
#include "coordinates/screen.h"

#include <algorithm>

using namespace om::coords;

namespace om {
namespace test {

TEST(Coords_Screen, ToGlobal)
{
	screenSystem ss(common::XY_VIEW, 128, 128, 1.0, Global(64));
	Screen s(128, 128, &ss);
	ASSERT_EQ(Global(128, 128, 64), s.ToGlobal());
	screenSystem ss2(common::XZ_VIEW, 128, 128, .5, Global(64));
	Screen s2(96, 96, &ss2);
	ASSERT_EQ(Global(128, 64, 128), s2.ToGlobal());
}

}} // namespace om::test::