#include "thrift/server.h"
#include <protocol/TBinaryProtocol.h>
#include <server/TSimpleServer.h>
#include <transport/TServerSocket.h>
#include <transport/TBufferTransports.h>

#include "project/project.h"
#include "project/details/projectVolumes.h"
#include "project/details/channelManager.h"
#include "project/details/segmentationManager.h"
#include "tiles/tile.h"
#include "volume/channel.h"

//#include "pipeline/getTileData.h"
//#include "pipeline/encode.hpp"

#include "network/writeTile.hpp"
#include "network/jpeg.h"

#include <fstream>
#include <sstream>

#include "b64/encode.h"

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using boost::shared_ptr;

using namespace om::common;

namespace om {
namespace server {

class serverHandler : virtual public serverIf
{
private:
    volume::channel * chan_;
    volume::segmentation * seg_;
//    jpeg::writer imageWriter_;

public:
    serverHandler()
    {
        project::Load("project.omni");
        chan_ = &project::Volumes().Channels().GetChannel(1);
        seg_ = &project::Volumes().Segmentations().GetSegmentation(1);
    }

    void get_volume_bounds(bbox& _return)
    {
        _return = chan_->CoordinateSystem().GetDataExtent();
    }

    void get_chan_tile(tile& _return, const vector3d& point, const int32_t mipLevel)
    {
        std::cout << "Called get_chan_tile:"
                  << point.x << "," << point.y << "," << point.z << ":" << mipLevel
                  << std::endl;

        network::writeTile writeTile;
        std::string uuid = writeTile.MakeTileFileChannel(point, mipLevel);
        std::string path = writeTile.FileName(chan_, uuid);
        std::ifstream in(path.c_str());
        std::stringstream ss;
        base64::encoder e;
        e.encode(in, ss);
        _return.data = ss.str();
    }

    void get_seg_tile(tile& _return, const vector3d& point,
                      const int32_t mipLevel, const int32_t segId)
    {
        std::cout << "Called get_seg_tile:"
                  << point.x << "," << point.y << "," << point.z << ":" << mipLevel
                  << std::endl;

        network::writeTile writeTile;
        std::string uuid = writeTile.MakeTileFileSegmentation(point, mipLevel);
        std::string path = writeTile.FileName(seg_, uuid);
        std::ifstream in(path.c_str());
        std::stringstream ss;
        base64::encoder e;
        e.encode(in, ss);
        _return.data = ss.str();
    }

    void get_seg_bbox(bbox& _return, const int32_t segId)
    {
        segment::segment s = seg_->Segments()->GetSegmentUnsafe(segId);
        _return = s.BoundingBox().toGlobalBbox();
    }

    int32_t get_seg_id(const vector3d& point)
    {
        return seg_->GetVoxelValue(point);
    }

    void get_seg_ids(std::vector<int32_t> & _return, const vector3d& point,
                     const double radius)
    {
        // Your implementation goes here
        printf("get_seg_ids\n");
    }

    double compare_results(const std::vector<result> & old_results, const result& new_result)
    {
        // Your implementation goes here
        printf("compare_results\n");
        return 0;
    }

};

}
}
