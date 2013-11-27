#include "handler/handler.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "datalayer/paths.hpp"
#include "volume/segmentation.h"
#include <memory>

using ::testing::_;
using namespace ::zi::mesh;

namespace om {
namespace test {

TEST(TaskSpawnTest, Case1) {
  file::Paths prePaths("/omniData/e2198/e2198_bc_s17_86_3_e24_101_16.omni");
  file::Paths postPaths("/omniData/e2198/e2198_bh_s18_86_16_e26_101_31.omni");
  volume::Segmentation pre(prePaths.Segmentation(1));
  volume::Segmentation post(postPaths.Segmentation(1));

  std::set<int> segs({1037975, 1039368, 1039480, 1061408, 1062016,
                      1083185, 1084004, 1085998, 1090552, 1092491,
                      1095443, 1096127, 1103096, 1103976, 1105366,
                      1105370, 1111813, 1118021, 1118624, 1125898});

  std::vector<std::map<int32_t, int32_t>> seedIds;
  handler::get_seeds(seedIds, pre, segs, post);
  EXPECT_GT(0, seedIds.size());
}

// TEST(TaskSpawnTest, Case2) {
//   file::Paths prePaths("/omniData/e2198/e2198_bc_s17_86_3_e24_101_16.omni");
//   file::Paths
// postPaths("/omniData/e2198/e2198_dl_s16_101_1_e24_116_16.omni");
//   volume::Segmentation pre(prePaths.Segmentation(1));
//   volume::Segmentation post(postPaths.Segmentation(1));

//   common::SegIDSet segs(
//       {854603, 861989, 861993, 883375, 886517, 887757, 1028531});

//   std::vector<std::map<int32_t, int32_t>> seedIds;
//   handler::get_seeds(seedIds, pre, segs, post);
// }

// TEST(TaskSpawnTest, Case3) {
//   file::Paths prePaths("/omniData/e2198/e2198_bc_s17_86_3_e24_101_16.omni");
//   file::Paths postPaths("/omniData/e2198/e2198_fl_s18_71_3_e24_86_16.omni");
//   volume::Segmentation pre(prePaths.Segmentation(1));
//   volume::Segmentation post(postPaths.Segmentation(1));

//   common::SegIDSet segs(
//       {513986, 515952, 519834, 520374, 530027, 530718, 532440, 551317});

//   std::vector<std::map<int32_t, int32_t>> seedIds;
//   handler::get_seeds(seedIds, pre, segs, post);
// }
}
}  // namespace om::test::
