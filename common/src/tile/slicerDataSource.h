#pragma once

#include "tile/dataSources.hpp"
#include "chunk/dataSources.hpp"

namespace om {
namespace tile {

class SlicerDataSource : public TileDS {
 public:
  SlicerDataSource(chunk::ChunkDS&, const coords::VolumeSystem&);
  virtual ~SlicerDataSource();

  std::shared_ptr<tile::TileVar> Get(const coords::Tile&,
                                     bool async = false) const;
  bool Put(const coords::Tile&, std::shared_ptr<tile::TileVar>,
           bool async = false);

  inline bool is_cache() const { return false; }
  inline bool is_persisted() const { return true; }

 private:
  chunk::ChunkDS& chunkDS_;
  const coords::VolumeSystem& system_;
};
}
}  // namespace om::tile::