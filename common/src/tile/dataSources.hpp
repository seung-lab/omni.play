#pragma once

#include "common/common.h"
#include "datalayer/dataSource.hpp"
#include "tile/tile.hpp"

namespace om {
namespace datalayer {
template <typename TKey, typename TValue>
class DataSourceHierarchy;
}
namespace tile {

typedef datalayer::IDataSource<coords::Tile, TileVar> TileDS;
typedef datalayer::DataSourceHierarchy<coords::Tile, TileVar> TileDSH;
}
}  // namespace om::tile::