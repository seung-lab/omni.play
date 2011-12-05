#include "gtest/gtest.h"

#include "common/common.h"
#include "handler/handler.h"
#include "thrift/server_types.h"

#include "boost/format.hpp"

namespace om {
namespace test {

class GetChanTile : public ::testing::Test
{
public:
    server::metadata meta;
    server::vector3d point_0, point_1, p0, p128, p256;

    GetChanTile()
    {
        std::string root = std::string("/omniData/e2198/e2198_a_s8_101_46_e16_116_61.omni.files/");

        meta.uri = str(boost::format("%1%%2%") % root
                       % "channels/channel1/0/volume.float.raw");
        meta.bounds.min.x = meta.bounds.min.y = meta.bounds.min.z = 0;
        meta.bounds.max.x = 1151;
        meta.bounds.max.y = 2047;
        meta.bounds.max.z = 2047;
        meta.resolution.x = meta.resolution.y = meta.resolution.z = 1;
        meta.chunkDims.x = meta.chunkDims.y = meta.chunkDims.z = 128;
        meta.type = server::dataType::UINT32;
        meta.mipLevel = 0;

        point_0.x = point_0.y = point_0.z = 10;
        point_1.x = point_1.y = point_1.z = 200;
        p0.x = p0.y = p0.z = 0;
        p128.x = p128.y = p128.z = 128;
        p256.x = p256.y = p256.z = 256;
    }
};

TEST_F(GetChanTile, InBounds)
{
    server::tile ret;
    handler::get_chan_tile(ret, meta, point_0, server::viewType::XY_VIEW);
    ASSERT_EQ(ret.view, server::viewType::XY_VIEW);
    ASSERT_EQ(ret.bounds.min, p0);
    ASSERT_EQ(ret.bounds.max, p128);
    ASSERT_GT(ret.data.size(), 0);

}

}
}
