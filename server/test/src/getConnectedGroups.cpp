#include "handler/handler.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "datalayer/paths.hpp"
#include "volume/segmentation.h"

using ::testing::_;
using namespace ::zi::mesh;

namespace om {
namespace test {

const server::group& get(const std::vector<server::group>& groups,
                         om::server::groupType::type type, int uid = 0,
                         bool dust = false) {
  auto iter = std::find_if(groups.begin(), groups.end(),
                           [type, uid, dust](const server::group& g) {
    return g.type == type && (g.user_id == uid || uid == 0) && g.dust == dust;
  });
  EXPECT_TRUE(iter != groups.end());
  return *iter;
}

// TEST(GetConnectedGroupsTest, Simple) {
//   file::Paths p("/omniData/e2198/e2198_a_s10_101_46_e17_116_61.omni");
//   volume::Segmentation vol(p, 1);

//   std::unordered_map<int, common::SegIDSet> groups;
//   groups[2].insert({1, 2, 3, 4, 5, 6, 7});
//   groups[3].insert({1, 3, 5, 7, 9, 11});
//   groups[4].insert({1, 3});

//   // Groups:
//   //  ALL:      1, 2, 3, 4, 5, 6, 7, 9, 11
//   //  AGREED:   1, 3
//   //  2 FOUND:  2, 4, 6
//   //  3 FOUND:  9, 11
//   //  4 MISSED: 5, 7

//   std::vector<server::group> _return;
//   handler::get_connected_groups(_return, vol, groups);

//   EXPECT_EQ(10, _return.size());
//   auto all = get(_return, om::server::groupType::ALL);
//   EXPECT_EQ(9, all.segments.size());

//   auto agreed = get(_return, om::server::groupType::AGREED);
//   EXPECT_EQ(2, agreed.segments.size());

//   auto two = get(_return, server::groupType::USER_FOUND, 2);
//   EXPECT_EQ(1, two.segments.size());

//   auto three = get(_return, server::groupType::USER_FOUND, 3);
//   EXPECT_EQ(2, three.segments.size());

//   auto four = get(_return, server::groupType::USER_MISSED, 4);
//   EXPECT_EQ(2, four.segments.size());
// }
}
}  // namespace om::test::
