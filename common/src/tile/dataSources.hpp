#pragma once

#include "common/common.h"
#include "datalayer/dataSource.hpp"
#include "datalayer/dataSourceHierarchy.hpp"
#include "tile/tile.hpp"

namespace om {
namespace tile {
typedef datalayer::IDataSource<coords::Tile, TileVar> TileDS;
typedef datalayer::DataSourceHierarchy<coords::Tile, TileVar> TileDSH;
}
namespace datalayer {
extern template class IDataSource<coords::Tile, tile::TileVar>;
extern template class DataSourceHierarchy<coords::Tile, tile::TileVar>;
}
}  // namespace om::tile::