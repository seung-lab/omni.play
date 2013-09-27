#include "handler/handler.h"
#include "handler/validate.hpp"

#include "common/common.h"
#include "volume/volume.h"
#include "volume/metadataManager.h"
#include "utility/timer.hpp"
#include "utility/convert.hpp"
#include "pipeline/mapData.hpp"
#include "pipeline/sliceTile.hpp"
#include "pipeline/jpeg.h"
#include "pipeline/encode.hpp"
#include "pipeline/bitmask.hpp"
#include "pipeline/png.hpp"
#include "pipeline/mask.hpp"
#include "tile/cachedDataSource.hpp"

using namespace boost;

namespace om {
namespace handler {

using namespace pipeline;
using namespace common;
using namespace utility;

void setTileBounds(server::tile& t, const coords::Tile& tileCoord,
                   const coords::VolumeSystem& system) {
  coords::Chunk cc = tileCoord.chunk();
  int depth = tileCoord.depth();
  coords::DataBbox bounds = cc.BoundingBox(system);

  server::vector3d min =
      Convert(twist(bounds.getMin().ToGlobal(), tileCoord.viewType()));
  server::vector3d max =
      Convert(twist(bounds.getMax().ToGlobal(), tileCoord.viewType()));

  min.z += depth * pow(2, cc.mipLevel());
  max.z = min.z;

  t.bounds.min = twist(min, tileCoord.viewType());
  t.bounds.max = twist(max, tileCoord.viewType());
}

void get_tiles(std::vector<server::tile>& _return, const volume::Volume& vol,
               const coords::Chunk& chunk, const common::ViewType view,
               const int32_t from, const int32_t to) {
  for (auto i = from; i < to; i++) {
    server::tile t;
    coords::Tile tileCoord(chunk, view, i);
    t.view = Convert(view);
    setTileBounds(t, tileCoord, vol.Coords());

    auto tile = vol.TileDS().Get(tileCoord);
    if (!tile) {
      throw IoException("Tile not found");
    }
    auto tileData = vol.Metadata().volumeType() == common::ObjectType::CHANNEL
                        ? *tile >> getTile() >> jpeg(128, 128) >> encode()
                        : *tile >> getTile() >> mask(0xFF000000) >>
                              png(128, 128, true) >> encode();

    Data<char> out = get<Data<char>>(tileData);
    t.data = vol.Metadata().volumeType() == common::ObjectType::CHANNEL
                 ? "data:image/jpeg;base64,"
                 : "data:image/png;base64,";
    t.data += std::string(out.data.get(), out.size);

    _return.push_back(t);
  }
}

}  // namespace handler
}  // namespace om
