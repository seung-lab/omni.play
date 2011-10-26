// This autogenerated skeleton file illustrates how to build a server.
// You should copy it to another filename to avoid overwriting it.

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
#include "b64/encode.h"

#include "jpeg/jpeg.h"

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
    jpeg::writer imageWriter_;
    base64::encoder e;

public:
    serverHandler()
    {
        project::Load("project.omni");
        chan_ = &project::Volumes().Channels().GetChannel(1);
        seg_ = &project::Volumes().Segmentations().GetSegmentation(1);
    }

    void get_volume_bounds(bbox& _return)
    {
        coords::globalBbox bounds = chan_->CoordinateSystem().GetDataExtent();
        _return.min.x = bounds.getMin().x;
        _return.min.y = bounds.getMin().y;
        _return.min.z = bounds.getMin().z;
        _return.max.x = bounds.getMax().x;
        _return.max.y = bounds.getMax().y;
        _return.max.z = bounds.getMax().z;
    }

    void get_chan_tile(tile& _return, const vector3d& point, const int32_t mipLevel)
    {
        std::cout << "Called get_chan_tile:"
                  << point.x << "," << point.y << "," << point.z << ":" << mipLevel
                  << std::endl;

        coords::globalCoord coord(point.x, point.y, point.z);
        coords::chunkCoord ccord = coord.toChunkCoord(*chan_, mipLevel);
        std::cout << "slicing" << std::endl;
        tiles::tile t(chan_, ccord, common::XY_VIEW, point.z);
        t.loadData();

        Vector3i dims = chan_->CoordinateSystem().MipedDataDimensions(mipLevel);
        int size;
        std::cout << "compressing" << std::endl;
        boost::shared_ptr<char> image = imageWriter_.write8(t.data(), dims.x, dims.y, size);
        char encoded[4 * (size / 3 + 1)]; // allocate enough space for the encoded data.
        std::cout << "encoding" << std::endl;
        e.encode(image.get(), size, encoded);
        std::string encStr(encoded);
        std::cout << "sending: " << encStr << std::endl;
        _return.data.swap(encStr);
    }

    void get_seg_tile(tile& _return, const vector3d& point,
                      const int32_t mipLevel, const int32_t segId)
    {
        std::cout << "Called get_seg_tile:"
                  << point.x << "," << point.y << "," << point.z
                  << std::endl;

        coords::globalCoord coord(point.x, point.y, point.z);
        coords::chunkCoord ccord = coord.toChunkCoord(*chan_, mipLevel);
        std::cout << "slicing" << std::endl;
        tiles::tile t(seg_, ccord, common::XY_VIEW, point.z);
        t.loadData();

        Vector3i dims = chan_->CoordinateSystem().MipedDataDimensions(mipLevel);
        int size;
        std::cout << "compressing" << std::endl;
        boost::shared_ptr<char> image = imageWriter_.write32(t.data(), dims.x, dims.y, size);
        char encoded[4 * (size / 3 + 1)]; // allocate enough space for the encoded data.
        std::cout << "encoding" << std::endl;
        e.encode(image.get(), size, encoded);
        _return.data = std::string(encoded);
        std::cout << "sending: " << _return.data << std::endl;
    }

    void get_seg_bbox(bbox& _return, const int32_t segId)
    {
//        segment::segment* s = seg_->Segments()->GetSegmentUnsafe(segId);
    }

    int32_t get_seg_id(const vector3d& point)
    {
        // Your implementation goes here
        printf("get_seg_id\n");
        return 0;
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
