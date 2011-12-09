#include <zi/zunit/zunit.hpp>

#include "common/common.h"
#include "handler/handler.h"
#include "thrift/server_types.h"

#include "boost/format.hpp"

ZiSUITE( Get_Tile_Tests );

namespace om {
namespace handler {

server::metadata meta;
server::vector3d point_0, point_1, p0, p128, p256;

void init()
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

}
}

ZiTEST(InBounds)
{
    using namespace om;
    using namespace om::handler;

    om::handler::init();
    server::tile ret;
    get_chan_tile(ret, meta, point_0, server::viewType::XY_VIEW);
    EXPECT_EQ(ret.view, server::viewType::XY_VIEW);

    EXPECT_EQ(ret.bounds.min.x, 0);
    EXPECT_EQ(ret.bounds.min.y, 0);
    EXPECT_EQ(ret.bounds.min.z, 10);
    EXPECT_EQ(ret.bounds.max.x, 128);
    EXPECT_EQ(ret.bounds.max.y, 128);
    EXPECT_EQ(ret.bounds.max.z, 10);

    EXPECT_GT(ret.data.size(), 0);
}
