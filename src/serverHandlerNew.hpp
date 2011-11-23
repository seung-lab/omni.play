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
                       const server::viewType::type view)
    {
        validateMetadata(vol);

        coords::volumeSystem coordSystem(vol);
        coords::globalCoord coord = point;
        coords::dataCoord dc = coord.toDataCoord(&coordSystem, vol.mipLevel);

        setTileBounds(_return, dc, view);

        using namespace pipeline;

        data_var encoded = mapData(vol.uri, vol.type) >> sliceTile(common::Convert(view), dc)
                                                      >> jpeg(128,128)
                                                      >> encode();

        data<char> out = boost::get<data<char> >(encoded);
        _return.data = std::string(out.data.get(), out.size);
    }

    void get_seg_tiles(std::map<std::string, tile> & _return,
                       const metadata& vol,
                       const int32_t segId,
                       const bbox& segBbox,
                       const viewType::type view)
    {
        validateMetadata(vol);

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
                    coords::dataCoord dc = coord.toDataCoord(&coordSystem, vol.mipLevel);

                    tile t;
                    makeSegTile(t, dataSrc, dc, view, segId);
                    std::stringstream ss;
                    ss << t.bounds.min.x << "-" << t.bounds.min.y << "-" << t.bounds.min.z;
                    _return[ss.str()] = t;
                }
            }
        }
    }

    int32_t get_seg_id(const metadata& vol, const vector3d& point)
    {
        validateMetadata(vol);

        coords::volumeSystem coordSystem(vol);

        coords::globalCoord coord = point;
        coords::dataCoord dc = coord.toDataCoord(&coordSystem, vol.mipLevel);

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
        validateMetadata(vol);

        // Your implementation goes here
        printf("get_seg_ids\n");
    }

    double compare_results(const std::vector<result> & old_results, const result& new_result)
    {
        // Your implementation goes here
        printf("compare_results\n");
        return 0;
    }

private:
    void validateMetadata(const metadata& meta)
    {
        if(!file::exists(meta.uri)) {
            throw common::argException("Cannot find requested volume.");
        }

        int factor = math::pow2int(meta.mipLevel);
/*
        std::size_t size = (meta.bounds.max.x - meta.bounds.min.x) *
            (meta.bounds.max.y - meta.bounds.min.y) *
            (meta.bounds.max.z - meta.bounds.min.z) /
            (factor * factor * factor);

        switch(meta.type)
        {
        case dataType::INT32:
        case dataType::UINT32:
        case dataType::FLOAT:
            size *= 4;
            break;
        }

        if(file::numBytes(meta.uri) != size) {
            throw common::argException("Incorrect file size given bounds, mip level and data type.");
        }
*/

        if(meta.resolution.x <= 0 || meta.resolution.y <= 0 || meta.resolution.z <= 0) {
            throw common::argException("Resolution must be greater than 0.");
        }

        if(meta.chunkDims.x <= 0 || meta.chunkDims.y <= 0 || meta.chunkDims.z <= 0) {
            throw common::argException("Chunk Dims must be greater than 0.");
        }
    }

    template<typename T>
    T twist(T vec, viewType::type view) const
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

    void makeSegTile(tile& t,
                     const pipeline::dataSrcs& src,
                     const coords::dataCoord& dc,
                     const viewType::type& view,
                     uint32_t segId) const
    {
        t.view = view;
        setTileBounds(t, dc, view);

        using namespace pipeline;

        data_var encoded = src >> sliceTile(common::Convert(view), dc)
                               >> bitmask(segId)
                               >> png(128,128)
                               >> encode();

        data<char> out = boost::get<data<char> >(encoded);
        t.data = std::string(out.data.get(), out.size);
    }

    inline void setTileBounds(tile& t,
                              const coords::dataCoord dc,
                              const viewType::type& view) const
    {
        coords::chunkCoord cc = dc.toChunkCoord();
        int depth = dc.toTileDepth(common::Convert(view));
        coords::dataBbox bounds = cc.chunkBoundingBox(dc.volume());

        vector3d min = twist(bounds.getMin().toGlobalCoord(), view);
        vector3d max = twist(bounds.getMax().toGlobalCoord(), view);

        min.z = depth;
        max.z = depth;

        t.bounds.min = twist(min, view);
        t.bounds.max = twist(max, view);
    }
};

}
}
