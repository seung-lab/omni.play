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
#include "segment/segment.h"
#include "segment/segments.h"

#include "pipeline/getTileData.hpp"
#include "pipeline/sliceTile.hpp"
#include "pipeline/jpeg.h"
#include "pipeline/encode.hpp"
#include "pipeline/utility.hpp"
//#include "pipeline/filter.hpp"
//#include "pipeline/bitmask.hpp"
//#include "pipeline/png.hpp"

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;


using boost::shared_ptr;
namespace om {
namespace server {

class serverHandler : virtual public serverIf {
public:
    serverHandler() {
        // Your initialization goes here
    }

    void get_chan_tile(server::tile& _return,
                       const server::metadata& vol,
                       const server::vector3d& point,
                       const int32_t mipLevel,
                       const server::viewType::type view)
    {
        coords::volumeSystem coordSystem(vol);

        coords::globalCoord coord = point;
        coords::dataCoord dc = coord.toDataCoord(&coordSystem, mipLevel);
        coords::chunkCoord cc = dc.toChunkCoord();
        common::viewType viewType = common::Convert(view);
        int depth = dc.toTileDepth(viewType);

        if(!coordSystem.ContainsMipChunkCoord(cc)) {
            throw common::argException("Requested data outside of volume.");
        }

        _return.view = view;

        coords::dataBbox bounds = cc.chunkBoundingBox(&coordSystem);
        _return.bounds.min = bounds.getMin().toGlobalCoord();
        _return.bounds.max = bounds.getMax().toGlobalCoord();

        using namespace pipeline;

        data_var encoded = mapData(vol.uri, vol.type) >> sliceTile(viewType, dc)
                                                      >> jpeg(128,128)
                                                      >> encode();

        data<char> out = boost::get<data<char> >(encoded);
        _return.data = std::string(out.data.get(), out.size);
    }

    void get_seg_tiles(std::vector<tile> & _return,
                       const metadata& vol,
                       const int32_t segId,
                       const bbox& segBbox,
                       const int32_t mipLevel,
                       const viewType::type view)
    {
/*        coords::volumeSystem coordSystem(vol);

        coords::globalCoord coord = point;
        coords::dataCoord dc = coord.toDataCoord(&coordSystem, mipLevel);
        coords::chunkCoord cc = dc.toChunkCoord();
        common::viewType viewType = common::Convert(view);
        int depth = dc.toTileDepth(viewType);

        if(!coordSystem.ContainsMipChunkCoord(cc)) {
            throw common::argException("Requested data outside of volume.");
        }

        _return.view = view;

        coords::dataBbox bounds = cc.chunkBoundingBox(&coordSystem);
        _return.bounds.min = bounds.getMin().toGlobalCoord();
        _return.bounds.max = bounds.getMax().toGlobalCoord();

        using namespace pipeline;

        data_var encoded = mapData(vol.uri, vol.type) >> sliceTile(viewType, dc)
                                                      >> jpeg(128,128)
                                                      >> encode();

        data<char> out = boost::get<data<char> >(encoded);
        _return.data = std::string(out.data.get(), out.size);*/
    }

    int32_t get_seg_id(const metadata& vol, const vector3d& point) {
        // Your implementation goes here
        printf("get_seg_id\n");
        return 0;
    }

    void get_seg_ids(std::vector<int32_t> & _return,
                     const metadata& vol,
                     const vector3d& point,
                     const double radius,
                     const viewType::type view)
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
