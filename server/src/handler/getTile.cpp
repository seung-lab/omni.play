#include "handler/handler.h"
#include "handler/validate.hpp"

#include "common/common.h"
#include "volume/volume.h"
#include "utility/timer.hpp"
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

void setTileBounds(server::tile& t,
                   const coords::data dc,
                   const common::viewType& view)
{
    coords::chunk cc = dc.toChunk();
    int depth = dc.toTileDepth(view);
    coords::dataBbox bounds = cc.chunkBoundingBox(dc.volume());

    server::vector3d min = common::twist(bounds.getMin().toGlobal(), view);
    server::vector3d max = common::twist(bounds.getMax().toGlobal(), view);

    min.z += depth;
    max.z = min.z;

    t.bounds.min = common::twist(min, view);
    t.bounds.max = common::twist(max, view);
}

void makeChanTile(server::tile& t,
                  const dataSrcs& src,
                  const coords::data& dc,
                  const common::viewType& view)
{
    t.view = common::Convert(view);
    setTileBounds(t, dc, view);

    data_var encoded = src >> sliceTile(view, dc)
                           >> jpeg(128,128)
                           >> encode();

    data<char> out = get<data<char> >(encoded);
    t.data = std::string(out.data.get(), out.size);
}

void makeSegTile(server::tile& t,
                 const dataSrcs& src,
                 const coords::data& dc,
                 const common::viewType& view)
{
    t.view = common::Convert(view);
    setTileBounds(t, dc, view);

    data_var encoded = src >> sliceTile(view, dc)
    					   >> mask(0xFF000000)
    					   >> png(128, 128, true)
    					   >> encode();

    data<char> out = get<data<char> >(encoded);
    t.data = std::string(out.data.get(), out.size);
}


void get_tiles(std::map<std::string, server::tile> & _return,
               const volume::volume& vol,
               const coords::globalBbox& segBbox,
               const common::viewType view,
               const int32_t mipLevel)
{
    coords::globalBbox bounds = segBbox;

    bounds.intersect(vol.Bounds());

    coords::global min = common::twist(bounds.getMin(), view);
    coords::global max = common::twist(bounds.getMax(), view);
    Vector3i dims = common::twist(vol.ChunkDims(), view);
    Vector3i res = common::twist(vol.Resolution(), view);

    for(int x = min.x; x <= max.x; x += dims.x * res.x) {
        for(int y = min.y; y <= max.y; y += dims.y * res.y) {
            for(int z = min.z; z <= max.z; z += res.z) // always depth when twisted
            {
                coords::global coord = common::twist(coords::global(x,y,z), view);
                coords::data dc = coord.toData(&vol.CoordSystem(), mipLevel);

                server::tile t;
                switch(vol.VolumeType())
                {
            	case server::volType::CHANNEL:
            		makeChanTile(t, vol.Data(mipLevel), dc, view);
            		break;
            	case server::volType::SEGMENTATION:
                	makeSegTile(t, vol.Data(mipLevel), dc, view);
                	break;
                }
                std::stringstream ss;
                ss << t.bounds.min.x << "-"
                   << t.bounds.min.y << "-"
                   << t.bounds.min.z;
                _return[ss.str()] = t;
            }
        }
    }
}

} // namespace handler
} // namespace om
