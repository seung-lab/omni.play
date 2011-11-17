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
#include "pipeline/bitmask.hpp"
#include "pipeline/png.hpp"
#include "pipeline/getSegIds.hpp"

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
        coords::volumeSystem coordSystem(vol);

        pipeline::mapData dataSrc(vol.uri, vol.type);

        vector3d min = twist(segBbox.min, view);
        vector3d max = twist(segBbox.max, view);
        vector3i dims = twist(vol.chunkDims, view);
        vector3i res = twist(vol.resolution, view);

        for(int x = min.x; x <= max.x; x += dims.x * res.x) {
            for(int y = min.y; y <= max.y; y += dims.y * res.y) {
                for(int z = min.z; z <= max.z; z += res.z) // always depth when twisted
                {
                    coords::globalCoord coord = twist(coords::globalCoord(x,y,z), view);
                    coords::dataCoord dc = coord.toDataCoord(&coordSystem, mipLevel);
                    tile t = makeSegTile(dataSrc, dc, view, segId);
                    _return.push_back(t);
                }
            }
        }
    }

private:
    template<typename T>
    T twist(T vec, viewType::type view)
    {
        T out;
        switch(view)
        {
        case viewType::XY_VIEW:
            out.x = vec.x;
            out.y = vec.y;
            out.z = vec.z;
            break;
        case viewType::XZ_VIEW:
            out.x = vec.x;
            out.z = vec.z;
            out.y = vec.y;
            break;
        case viewType::ZY_VIEW:
            out.z = vec.z;
            out.y = vec.y;
            out.x = vec.x;
            break;
        }

        return out;
    }

    tile makeSegTile(const pipeline::dataSrcs& src,
                     const coords::dataCoord& dc,
                     const viewType::type& view,
                     uint32_t segId) const
    {
        using namespace pipeline;

        coords::chunkCoord cc = dc.toChunkCoord();
        common::viewType viewType = common::Convert(view);
        int depth = dc.toTileDepth(viewType);

        tile t;
        t.view = view;

        coords::dataBbox bounds = cc.chunkBoundingBox(dc.volume());
        t.bounds.min = bounds.getMin().toGlobalCoord();
        t.bounds.max = bounds.getMax().toGlobalCoord();

        data_var encoded = src >> sliceTile(viewType, dc) >> bitmask(segId)
                               >> png(128,128) >> encode();

        data<char> out = boost::get<data<char> >(encoded);
        t.data = std::string(out.data.get(), out.size);

        return t;
    }

public:
    int32_t get_seg_id(const metadata& vol, const vector3d& point)
    {
        coords::volumeSystem coordSystem(vol);

        coords::globalCoord coord = point;
        coords::dataCoord dc = coord.toDataCoord(&coordSystem, 0);

        using namespace pipeline;

        mapData dataSrc(vol.uri, vol.type);

        data_var id = dataSrc >> getSegIds(dc, 0);

        return boost::get<data<uint32_t> >(id).data.get()[0];
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
