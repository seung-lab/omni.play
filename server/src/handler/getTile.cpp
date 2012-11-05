#include "handler/handler.h"
#include "handler/validate.hpp"

#include "common/common.h"
#include "volume/volume.h"
#include "utility/timer.hpp"
#include "utility/convert.hpp"
#include "pipeline/mapData.hpp"
#include "pipeline/sliceTile.hpp"
#include "pipeline/jpeg.h"
#include "pipeline/encode.hpp"
#include "pipeline/bitmask.hpp"
#include "pipeline/png.hpp"
#include "pipeline/mask.hpp"

using namespace std;
using namespace boost;

namespace om {
namespace handler {

using namespace pipeline;
using namespace common;
using namespace utility;

void setTileBounds(server::tile& t,
                   const coords::Data dc,
                   const viewType& view)
{
    coords::Chunk cc = dc.ToChunk();
    int depth = dc.ToTileDepth(view);
    coords::DataBbox bounds = cc.BoundingBox(dc.volume());

    server::vector3d min = Convert(twist(bounds.getMin().ToGlobal(), view));
    server::vector3d max = Convert(twist(bounds.getMax().ToGlobal(), view));

    min.z += depth;
    max.z = min.z;

    t.bounds.min = twist(min, view);
    t.bounds.max = twist(max, view);
}



void get_chan_tile(server::tile& _return,
                   const volume::volume& vol,
                   const coords::Global& point,
                   const viewType view)
{
    if(!vol.Bounds().contains(point)) {
        throw argException("Requested Channel Tile outside bounds of volume.");
    }

    coords::Data dc = point.ToData(&vol.CoordSystem(), vol.MipLevel());

    setTileBounds(_return, dc, view);
    _return.view = Convert(view);

    data_var encoded = vol.Data() >> sliceTile(view, dc)
                                  >> jpeg(128,128)
                                  >> encode();

    data<char> out = get<data<char> >(encoded);
    _return.data = std::string(out.data.get(), out.size);
}

void makeSegTile(server::tile& t,
                 const dataSrcs& src,
                 const coords::Data& dc,
                 const viewType& view,
                 uint32_t segId)
{
    t.view = Convert(view);
    setTileBounds(t, dc, view);

    data_var encoded = src >> sliceTile(view, dc)
    					   >> mask(0xFF000000)
    					   >> png(128, 128, true)
    					   >> encode();

    data<char> out = get<data<char> >(encoded);
    t.data = std::string(out.data.get(), out.size);
}


void get_seg_tiles(std::map<std::string, server::tile> & _return,
                   const volume::volume& vol,
                   const int32_t segId,
                   const coords::GlobalBbox& segBbox,
                   const viewType view)
{
    coords::GlobalBbox bounds = segBbox;

    bounds.intersect(vol.Bounds());

    coords::Global min = twist(bounds.getMin(), view);
    coords::Global max = twist(bounds.getMax(), view);
    Vector3i dims = twist(vol.ChunkDims(), view);
    Vector3i res = twist(vol.Resolution(), view);

    for(int x = min.x; x <= max.x; x += dims.x * res.x) {
        for(int y = min.y; y <= max.y; y += dims.y * res.y) {
            for(int z = min.z; z <= max.z; z += res.z) // always depth when twisted
            {
                coords::Global coord = twist(coords::Global(x,y,z), view);
                coords::Data dc = coord.ToData(&vol.CoordSystem(), vol.MipLevel());

                server::tile t;
                makeSegTile(t, vol.Data(), dc, view, segId);
                std::stringstream ss;
                ss << t.bounds.min.x << "-"
                   << t.bounds.min.y << "-"
                   << t.bounds.min.z << "-"
                   << segId;
                _return[ss.str()] = t;
            }
        }
    }
}

} // namespace handler
} // namespace om
