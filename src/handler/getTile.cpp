#include "handler/handler.h"
#include "handler/validate.hpp"

#include "common/common.h"
#include "utility/timer.hpp"
#include "pipeline/mapData.hpp"
#include "pipeline/sliceTile.hpp"
#include "pipeline/jpeg.h"
#include "pipeline/encode.hpp"
#include "pipeline/bitmask.hpp"
#include "pipeline/png.hpp"

using namespace std;
using namespace boost;

namespace om {
namespace handler {

using namespace pipeline;

void setTileBounds(server::tile& t,
                   const coords::data dc,
                   const server::viewType::type& view)
{
    coords::chunk cc = dc.toChunk();
    int depth = dc.toTileDepth(common::Convert(view));
    coords::dataBbox bounds = cc.chunkBoundingBox(dc.volume());

    server::vector3d min = common::twist(bounds.getMin().toGlobal(), view);
    server::vector3d max = common::twist(bounds.getMax().toGlobal(), view);

    min.z += depth;
    max.z = min.z;

    t.bounds.min = common::twist(min, view);
    t.bounds.max = common::twist(max, view);
}



void get_chan_tile(server::tile& _return,
                   const server::metadata& vol,
                   const server::vector3d& point,
                   const server::viewType::type view)
{
    utility::timer t;
    validateMetadata(vol);

    coords::volumeSystem coordSystem(vol);
    coords::global coord = point;
    coords::data dc = coord.toData(&coordSystem, vol.mipLevel);

    setTileBounds(_return, dc, view);

    data_var encoded = mapData(vol.uri, vol.type) >> sliceTile(common::Convert(view), dc)
                                                  >> jpeg(128,128)
                                                  >> encode();

    data<char> out = get<data<char> >(encoded);
    _return.data = std::string(out.data.get(), out.size);

    cout << "get_chan_tile done: " << t.s_elapsed() << " seconds" << endl;
}

void makeSegTile(server::tile& t,
                 const dataSrcs& src,
                 const coords::data& dc,
                 const server::viewType::type& view,
                 uint32_t segId)
{
    t.view = view;
    setTileBounds(t, dc, view);

    data_var encoded = src >> sliceTile(common::Convert(view), dc)
                           >> bitmask(segId)
                           >> png(128,128)
                           >> encode();

    data<char> out = get<data<char> >(encoded);
    t.data = std::string(out.data.get(), out.size);
}


void get_seg_tiles(std::map<std::string, server::tile> & _return,
                   const server::metadata& vol,
                   const int32_t segId,
                   const server::bbox& segBbox,
                   const server::viewType::type view)
{
    utility::timer t;

    validateMetadata(vol);

    coords::volumeSystem coordSystem(vol);

    mapData dataSrc(vol.uri, vol.type);

    server::vector3d min = common::twist(segBbox.min, view);
    server::vector3d max = common::twist(segBbox.max, view);
    server::vector3i dims = common::twist(vol.chunkDims, view);
    server::vector3i res = common::twist(vol.resolution, view);

    for(int x = min.x; x <= max.x; x += dims.x * res.x) {
        for(int y = min.y; y <= max.y; y += dims.y * res.y) {
            for(int z = min.z; z <= max.z; z += res.z) // always depth when twisted
            {
                coords::global coord = common::twist(coords::global(x,y,z), view);
                coords::data dc = coord.toData(&coordSystem, vol.mipLevel);

                server::tile t;
                makeSegTile(t, dataSrc, dc, view, segId);
                std::stringstream ss;
                ss << t.bounds.min.x << "-"
                   << t.bounds.min.y << "-"
                   << t.bounds.min.z << "-"
                   << segId;
                _return[ss.str()] = t;
            }
        }
    }

    std::cout << "get_seg_tiles done: " << t.s_elapsed() << " seconds" << std::endl;
}

} // namespace handler
} // namespace om
