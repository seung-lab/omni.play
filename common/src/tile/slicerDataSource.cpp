#include "tile/slicerDataSource.h"
#include "chunk/rawChunkSlicer.hpp"

namespace om {
namespace tile {

SlicerDataSource::SlicerDataSource(chunk::ChunkDS& chunkDS,
                                   const coords::VolumeSystem& system)
    : chunkDS_(chunkDS), system_(system) {}

SlicerDataSource::~SlicerDataSource() {}

class TileSlicer
    : public boost::static_visitor<std::shared_ptr<tile::TileVar>> {
 public:
  TileSlicer(coords::Tile coord, const coords::VolumeSystem& system)
      : coord_(coord), system_(system) {}

  template <typename T>
  std::shared_ptr<tile::TileVar> operator()(const chunk::Chunk<T>& in) const {
    chunk::rawChunkSlicer<T> slicer(system_.ChunkDimensions().x,
                                    in.data().get());
    return std::make_shared<tile::TileVar>(
        tile::Tile<T>(coord_, system_,
                      slicer.GetCopyOfTile(coord_.viewType(), coord_.depth())));
  }

 private:
  coords::Tile coord_;
  const coords::VolumeSystem& system_;
};

std::shared_ptr<tile::TileVar> SlicerDataSource::Get(const coords::Tile& coord,
                                                     bool async) const {
  auto chunk = chunkDS_.Get(coord.chunk());
  if (!chunk) {
    return std::shared_ptr<tile::TileVar>();
  }
  return boost::apply_visitor(TileSlicer(coord, system_), *chunk);
}

class TileWriter : public boost::static_visitor<bool> {
 public:
  TileWriter(coords::Tile coord, const coords::VolumeSystem& system)
      : coord_(coord), system_(system) {}

  template <typename T>
  bool operator()(chunk::Chunk<T>& in, tile::Tile<T>& tile) const {
    chunk::rawChunkSlicer<T> slicer(system_.ChunkDimensions().x,
                                    in.data().get());
    slicer.WriteTileToChunk(coord_.viewType(), coord_.depth(),
                            tile.data().get());
    return true;
  }

  template <typename T, typename U>
  bool operator()(chunk::Chunk<T>&, tile::Tile<U>&) const {
    log_errors << "Writing incorrect type of data to chunk";
    return false;
  }

 private:
  coords::Tile coord_;
  const coords::VolumeSystem& system_;
};

bool SlicerDataSource::Put(const coords::Tile& coord,
                           std::shared_ptr<tile::TileVar> tile, bool async) {
  auto chunk = chunkDS_.Get(coord.chunk());
  if (!chunk) {
    return false;
  }
  return boost::apply_visitor(TileWriter(coord, system_), *chunk, *tile);
}
}
}  // namespace om::tile::