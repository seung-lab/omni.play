#include "handler/handler.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "datalayer/paths.hpp"
#include "volume/segmentation.h"

using ::testing::_;
using namespace ::zi::mesh;

namespace om {
namespace test {

TEST(GetConnectedGroupsTest, Simple) {
  file::Paths p("/omniData/e2198/e2198_a_s10_101_46_e17_116_61.omni");
  volume::Segmentation vol(p.Segmentation(1));

  std::unordered_map<int, common::SegIDSet> groups;
  groups[1].insert({1, 2, 3, 4, 5});
  groups[2].insert({1, 3, 5, 7, 9});

  std::vector<server::group> _return;
  handler::get_connected_groups(_return, vol, groups);
  EXPECT_EQ(_return.size(), 6);
}
}
}  // namespace om::test::
