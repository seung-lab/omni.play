#include "tile/dataSources.hpp"

namespace om {
namespace datalayer {
template class IDataSource<coords::Tile, tile::TileVar>;
template class DataSourceHierarchy<coords::Tile, tile::TileVar>;
}
}  // namespace om::tile::