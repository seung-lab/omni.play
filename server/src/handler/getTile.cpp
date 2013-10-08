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

void setTileBounds(server::tile& t, const coords::data dc,
                   const common::viewType& view) {
  coords::chunk cc = dc.toChunk();
  int depth = dc.toTileDepth(view);
  coords::dataBbox bounds = cc.chunkBoundingBox(dc.volume());

  server::vector3d min = common::twist(bounds.getMin().toGlobal(), view);
  server::vector3d max = common::twist(bounds.getMax().toGlobal(), view);

  min.z += depth * pow(2, dc.level());
  max.z = min.z;

  t.bounds.min = common::twist(min, view);
  t.bounds.max = common::twist(max, view);
}

void makeChanTile(server::tile& t, const dataSrcs& src, const coords::data& dc,
                  const common::viewType& view) {
  t.view = common::Convert(view);
  setTileBounds(t, dc, view);

  data_var encoded = src >> sliceTile(view, dc) >> jpeg(128, 128) >> encode();

  data<char> out = get<data<char> >(encoded);
  t.data = "data:image/jpeg;base64," + std::string(out.data.get(), out.size);
}

void makeSegTile(server::tile& t, const dataSrcs& src, const coords::data& dc,
                 const common::viewType& view) {
  t.view = common::Convert(view);
  setTileBounds(t, dc, view);

  data_var encoded = src >> sliceTile(view, dc) >> mask(0xFF000000) >>
                     png(128, 128, true) >> encode();

  data<char> out = get<data<char> >(encoded);
  t.data = "data:image/png;base64," + std::string(out.data.get(), out.size);
}

void get_tiles(std::vector<server::tile>& _return, const volume::volume& vol,
               const coords::chunk& chunk, const common::viewType view,
               const int32_t from, const int32_t to) {
  for (int i = from; i < to; ++i) {
    coords::data dc = chunk.toData(&vol.CoordSystem());
    coords::data twisted = twist(dc, view);
    twisted.z += i;
    dc = twist(twisted, view);

    if (!dc.isInVolume()) {
      continue;
    }

    server::tile t;
    switch (vol.VolumeType()) {
      case server::volType::CHANNEL:
        makeChanTile(t, vol.Data(chunk.Level), dc, view);
        break;
      case server::volType::SEGMENTATION:
        makeSegTile(t, vol.Data(chunk.Level), dc, view);
        break;
    }
    _return.push_back(t);
  }
}

}  // namespace handler
}  // namespace om
