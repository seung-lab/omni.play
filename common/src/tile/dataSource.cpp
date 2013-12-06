#include "tile/dataSource.h"
#include "pipeline/sliceTile.hpp"
#include "tile/tile.hpp"
#include "datalayer/paths.hpp"

namespace om {
namespace tile {

using namespace pipeline;

FileDataSource::FileDataSource(const file::path& volRoot,
                               const common::DataType& dataType,
                               const coords::VolumeSystem& coordSystem)
    : volRoot_(volRoot), dataType_(dataType), coordSystem_(coordSystem) {}

std::shared_ptr<tile::TileVar> FileDataSource::Get(const coords::Tile& coord,
                                                   bool async) {
  try {
    auto file = files_.Get(fileName(coord.chunk().mipLevel()), dataType_);
    return file >> TileSlicer(coord, coordSystem_);
  }
  catch (Exception e) {
    log_errors << "Error reading from TileDS: " << e.what();
    return std::shared_ptr<tile::TileVar>();
  }
}

bool FileDataSource::Put(const coords::Tile&, std::shared_ptr<tile::TileVar>,
                         bool async) {
  throw om::NotImplementedException("fixme");
}

std::string FileDataSource::fileName(int mipLevel) {
  return (volRoot_ / file::Paths::Vol::Data(mipLevel, dataType_)).string();
}
}
}  // namespace om::tile::
