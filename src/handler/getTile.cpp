#include "handler/handler.h"

#include "common/common.h"
#include "utility/timer.hpp"
#include "pipeline/mapData.hpp"
#include "pipeline/sliceTile.hpp"
#include "pipeline/jpeg.h"
#include "pipeline/encode.hpp"
#include "pipeline/bitmask.hpp"
#include "pipeline/png.hpp"
#include "volume/volume.h"
#include "tiles/tile.h"

using namespace std;
using namespace boost;

namespace om {
namespace handler {

using namespace pipeline;

void get_chan_tile(server::tile& _return,
                   volume::volume& vol,
                   const coords::global& point,
                   const common::viewType view)
{
    tiles::Tile t = vol.GetChanTile(point, view);
    t.WriteJpeg(_return);
}

void get_seg_tiles(std::map<std::string, server::tile> & _return,
                   volume::volume& vol,
                   const int32_t segId,
                   const coords::globalBbox& segBbox,
                   const common::viewType view)
{
    coords::globalBbox bounds = segBbox;

    bounds.intersect(vol.CoordSystem().GetDataExtent());

    coords::global min = common::twist(bounds.getMin(), view);
    coords::global max = common::twist(bounds.getMax(), view);
    Vector3i dims = common::twist(vol.CoordSystem().GetChunkDimensions(), view);
    Vector3i res = common::twist(vol.CoordSystem().GetResolution(), view);

    for(int x = min.x; x <= max.x; x += dims.x * res.x) {
        for(int y = min.y; y <= max.y; y += dims.y * res.y) {
            for(int z = min.z; z <= max.z; z += res.z) // always depth when twisted
            {
                coords::global coord = common::twist(coords::global(x,y,z), view);
                tiles::Tile t = vol.GetSegTile(coord, view);

                server::tile out;
                t.WriteBitmaskedPng(segId, out);

                std::stringstream ss;
                ss << out.bounds.min.x << "-"
                   << out.bounds.min.y << "-"
                   << out.bounds.min.z << "-"
                   << segId;
                _return[ss.str()] = out;
            }
        }
    }
}

} // namespace handler
} // namespace om
