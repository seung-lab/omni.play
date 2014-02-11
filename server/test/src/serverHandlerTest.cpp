#include "handler/handler.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "datalayer/paths.hpp"
#include "volume/metadataDataSource.hpp"
#include "volume/volume.h"
#include "volume/segmentation.h"
#include <memory>

using ::testing::_;
using namespace ::zi::mesh;

namespace om {
namespace test {

class ServerHandlerTest : public ::testing::Test {
 public:
  ServerHandlerTest() {
    file::Paths p("test/data/test.omni.files/");
    channel.reset(new volume::Volume(p.Channel(1)));
    segmentation.reset(new volume::Segmentation(p, 1));
  }

 protected:
  std::unique_ptr<volume::Volume> channel;
  std::unique_ptr<volume::Segmentation> segmentation;
};

TEST_F(ServerHandlerTest, get_tiles) {
  std::vector<server::tile> ret;
  const coords::Chunk chunk(0, vmml::Vector3i(0));

  handler::get_tiles(ret, *channel, chunk, common::XY_VIEW, 0, 3);
  EXPECT_TRUE(ret.size() > 0);
  handler::get_tiles(ret, *channel, chunk, common::XZ_VIEW, 0, 3);
  EXPECT_TRUE(ret.size() > 0);

  handler::get_tiles(ret, *segmentation, chunk, common::XY_VIEW, 0, 3);
  EXPECT_TRUE(ret.size() > 0);
  handler::get_tiles(ret, *segmentation, chunk, common::XZ_VIEW, 0, 3);
  EXPECT_TRUE(ret.size() > 0);
}

TEST_F(ServerHandlerTest, get_seg_list_data) {
  file::Paths p(
      "/omniweb_data/x06/y59/"
      "x06y59z28_s1587_13491_6483_e1842_13746_6738.omni");
  volume::Segmentation seg(p, 1);

  std::map<int, server::segData> ret;
  std::set<int32_t> segIds;
  segIds.insert(5);
  segIds.insert(7);
  segIds.insert(9);
  segIds.insert(11);

  handler::get_seg_list_data(ret, seg, segIds);
  for (const auto& pair : ret) {
    const auto& data = pair.second;

    EXPECT_GT(data.bounds.max.x, data.bounds.min.x);
    EXPECT_GT(data.bounds.max.y, data.bounds.min.y);
    EXPECT_GT(data.bounds.max.z, data.bounds.min.z);
    EXPECT_GT(data.size, 0);
  }
}

TEST_F(ServerHandlerTest, get_mesh) {
  std::string ret;
  const server::vector3i chunk;
  int32_t segId = 100;

  handler::get_mesh(ret, *segmentation, chunk, 0, segId);
  EXPECT_TRUE(ret.size() > 0);
  handler::get_mesh(ret, *segmentation, chunk, 1, segId);
  EXPECT_TRUE(ret.size() > 0);
}

TEST_F(ServerHandlerTest, get_obj) {
  std::string ret;
  const server::vector3i chunk;
  int32_t segId = 100;

  handler::get_obj(ret, *segmentation, chunk, 0, segId);
  EXPECT_TRUE(ret.size() > 0);
  handler::get_obj(ret, *segmentation, chunk, 1, segId);
  EXPECT_TRUE(ret.size() > 0);
}

TEST_F(ServerHandlerTest, get_mst) {
  std::vector<server::affinity> ret;

  handler::get_mst(ret, *segmentation);
  EXPECT_TRUE(ret.size() > 0);
}
}
}  // namespace om::test::
