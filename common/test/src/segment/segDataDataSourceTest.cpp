#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "utility/UUID.hpp"
#include "segment/fileDataSource.hpp"
#include "segment/dataSources.hpp"
#include "volume/segmentation.h"

using namespace om::segment;

namespace om {
namespace test {

TEST(Segment_SegDataDS, Get) {
  file::Paths p("/omniData/e2198/e2198_a_s10_101_46_e17_116_61.omni");
  FileDataSource fds(p.UserPaths("_default").Segments(1));

  auto values = fds.Get(0);
  ASSERT_TRUE((bool)values);
  ASSERT_EQ(values->Values.size(), 100000);

  values = fds.Get(100);
  ASSERT_FALSE((bool)values);
}

TEST(Segment_SegDataDS, Put) {}

TEST(Segment_SegListTypeDataDS, Get) {
  file::Paths p("/omniData/e2198/e2198_a_s10_101_46_e17_116_61.omni");
  ListTypeFileDataSource ltfds(p.UserPaths("_default").Segments(1));

  auto values = ltfds.Get(0);
  ASSERT_TRUE((bool)values);
  ASSERT_EQ(values->Values.size(), 100000);

  values = ltfds.Get(100);
  ASSERT_FALSE((bool)values);
}

TEST(Segment_SegListTypeDataDS, Put) {}

#undef URI
}
}  // namespace om::test::
