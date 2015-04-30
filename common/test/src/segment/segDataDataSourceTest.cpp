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
  file::Paths p("../../test_data/x06y59z28_s1587_13491_6483_e1842_13746_6738.omni");
  FileDataSource fds(p.UserPaths("_default").Segments(1));

  auto values = fds.Get(0);
  ASSERT_TRUE((bool)values);
  ASSERT_EQ(values->Values.size(), 100000);

  //Because page 100 doesn't exist a null pointer will be returned.
  values = fds.Get(100);
  ASSERT_FALSE((bool)values);
}

TEST(Segment_SegDataDS, Put) {}

TEST(Segment_SegListTypeDataDS, Get) {
  file::Paths p("../../test_data/x06y59z28_s1587_13491_6483_e1842_13746_6738.omni");
  ListTypeFileDataSource ltfds(p.UserPaths("_default").Segments(1));

  auto values = ltfds.Get(0);
  ASSERT_TRUE((bool)values);
  ASSERT_EQ(values->Values.size(), 100000);

  //Because page 100 doesn't exist a null pointer will be returned.
  values = ltfds.Get(100);
  ASSERT_FALSE((bool)values);
}

TEST(Segment_SegListTypeDataDS, Put) {}

#undef URI
}
}  // namespace om::test::
