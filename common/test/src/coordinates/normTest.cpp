#include "gtest/gtest.h"

#include "common/common.h"
#include "coordinates/norm.h"

#include <algorithm>

using namespace om::coords;

namespace om {
namespace test {

TEST(Coords_Norm, ToGlobal)
{
	VolumeSystem vs(Vector3i(1024));
	Norm n(.125, .25, .5, &vs);
	ASSERT_EQ(Global(128, 256, 512), n.ToGlobal());
}

TEST(Coords_Norm, ToData)
{
	VolumeSystem vs(Vector3i(1024));
	Norm n(.125, .25, .5, &vs);
	ASSERT_EQ(Data(128, 256, 512, &vs, 0), n.ToData(0));
	ASSERT_EQ(Data(64, 128, 256, &vs, 1), n.ToData(1));
}


}} // namespace om::test::