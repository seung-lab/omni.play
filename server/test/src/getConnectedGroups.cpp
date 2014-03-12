#include "handler/handler.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "datalayer/paths.hpp"
#include "volume/segmentation.h"

using ::testing::_;
using namespace ::zi::mesh;

namespace om {
namespace test {

const server::group& getByType(const std::vector<server::group>& groups,
                               om::server::groupType::type type) {
  auto iter =
      std::find_if(groups.begin(), groups.end(),
                   [type](const server::group& g) { return g.type == type; });
  EXPECT_TRUE(iter != groups.end());
  return *iter;
}

const server::group& getByID(const std::vector<server::group>& groups, int id) {
  auto iter =
      std::find_if(groups.begin(), groups.end(),
                   [id](const server::group& g) { return g.user_id == id; });
  EXPECT_TRUE(iter != groups.end());
  return *iter;
}

TEST(GetConnectedGroupsTest, Simple) {
  file::Paths p("/omniData/e2198/e2198_a_s10_101_46_e17_116_61.omni");
  volume::Segmentation vol(p, 1);

  std::unordered_map<int, common::SegIDSet> groups;
  groups[2].insert({1, 2, 3, 4, 5});
  groups[3].insert({1, 3, 5, 7, 9});
  groups[4].insert({1, 3, 5});

  std::vector<server::group> _return;
  handler::get_connected_groups(_return, vol, groups);

  EXPECT_EQ(7, _return.size());
  auto all = getByType(_return, om::server::groupType::ALL);
  EXPECT_EQ(1, all.groups.size());
  EXPECT_EQ(7, all.groups.front().size());

  auto agreed = getByType(_return, om::server::groupType::AGREED);
  EXPECT_EQ(1, agreed.groups.size());
  EXPECT_EQ(3, agreed.groups.front().size());

  auto partial = getByType(_return, om::server::groupType::PARTIAL);
  EXPECT_EQ(1, partial.groups.size());
  EXPECT_EQ(0, partial.groups.front().size());

  auto dust = getByType(_return, om::server::groupType::DUST);
  EXPECT_EQ(1, dust.groups.size());
  EXPECT_EQ(1, dust.groups.front().size());

  auto two = getByID(_return, 2);
  EXPECT_EQ(1, two.groups.size());
  EXPECT_EQ(1, two.groups.front().size());

  auto three = getByID(_return, 3);
  EXPECT_EQ(1, three.groups.size());
  EXPECT_EQ(2, three.groups.front().size());

  auto four = getByID(_return, 4);
  EXPECT_EQ(0, four.groups.size());
}
}
}  // namespace om::test::
