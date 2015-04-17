#include "gtest/gtest.h"

#include "common/common.h"
#include "coordinates/coordinates.h"
#include "utility/timer.hpp"

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

TEST(Coords_Global, DataToGlobal_RoundTrip1) {
  VolumeSystem vs(Vector3i(1024), Vector3i::ZERO, Vector3i(7, 7, 40));
  Data d(1023, 1023, 236, vs, 0);
  Global g(7161, 7161, 236 * 40);
  ASSERT_EQ(d.ToGlobal().ToData(vs, 0), d);
  ASSERT_EQ(g.ToData(vs, 0), d);
  ASSERT_EQ(g, d.ToGlobal());
}

TEST(Coords_Global, DataToGlobal_RoundTrip2) {
  int min = 1;
  int max = 1e4;

  for (int i = min; i < max; i += 3) {
    VolumeSystem vs(Vector3i(max), Vector3i::ZERO, Vector3i(i, i + 1, i + 2));
    // std::cout << i << std::endl;
    for (int j = 1; j < max; j++) {
      Data d(j, j, j, vs, 0);
      Global g(j * i, j * (i + 1), j * (i + 2));
      ASSERT_EQ(d.ToGlobal().ToData(vs, 0), d);
      ASSERT_EQ(g.ToData(vs, 0), d);
      ASSERT_EQ(g, d.ToGlobal());
    }
  }
}

Data GlobalToData_NoRounding(const Global& g, const VolumeSystem& vol,
                             const int mipLevel) {
  const vmml::Vector4f Global(g.x, g.y, g.z, 1);
  vmml::Vector3f dataCf = vol.GlobalToDataMat(mipLevel) * Global;
  return Data(dataCf, vol, mipLevel);
}

TEST(Coords_Global, Benchmark_ToData_Rounding) {
  int min = 1;
  int max = 3e4;

  utility::timer t;
  t.start();
  for (int i = min; i < max; i += 3) {
    VolumeSystem vs(Vector3i(max), Vector3i::ZERO, Vector3i(i, i + 1, i + 2));
    for (int j = 1; j < max; j++) {
      Global g(j * i, j * (i + 1), j * (i + 2));
      g.ToData(vs, 0);
    }
  }
  t.Print("ToData w rounding");

  t.reset();
  t.start();
  for (int i = min; i < max; i += 3) {
    VolumeSystem vs(Vector3i(max), Vector3i::ZERO, Vector3i(i, i + 1, i + 2));
    for (int j = 1; j < max; j++) {
      Global g(j * i, j * (i + 1), j * (i + 2));
      // Apparently having defined this function in this same translation unit
      // also significantly improves performance. Ideal apple to apple
      // comparison would have it defined in anther file.
      om::test::GlobalToData_NoRounding(g, vs, 0);
    }
  }
  t.Print("ToData w/o rounding");
  t.reset();
}
}
}  // namespace om::test::
