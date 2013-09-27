#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "utility/UUID.hpp"
#include "segment/fileDataSource.hpp"
#include "segment/dataSources.hpp"

using namespace om::segment;

namespace om {
namespace test {

#define URI \
  "test/data/test.omni.files/users/_default/segmentations/segmentation1/"

TEST(Segment_SegDataDS, Get) {
  FileDataSource fds(URI);

  auto values = fds.Get(0);
  ASSERT_TRUE((bool)values);
  ASSERT_EQ(values->Values.size(), 100000);

  values = fds.Get(100);
  ASSERT_FALSE((bool)values);
}

TEST(Segment_SegDataDS, Put) {}

TEST(Segment_SegListTypeDataDS, Get) {
  ListTypeFileDataSource ltfds(URI);

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
