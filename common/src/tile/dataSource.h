#pragma once

#include "datalayer/dataSource.hpp"
#include "datalayer/filePtrCache.hpp"
#include "tile/dataSources.hpp"

namespace om {
namespace coords {
class VolumeSystem;
}
namespace tile {

class FileDataSource : public TileDS {
 public:
  FileDataSource(const file::path&, const common::DataType&,
                 const coords::VolumeSystem&);

  std::shared_ptr<tile::TileVar> Get(const coords::Tile&, bool async = false);
  bool Put(const coords::Tile&, std::shared_ptr<tile::TileVar>,
           bool async = false);

  inline bool is_cache() const { return false; }
  inline bool is_persisted() const { return true; }

 private:
  std::string fileName(int);

  const file::path volRoot_;
  const common::DataType dataType_;
  datalayer::VarFilePtrCache files_;
  const coords::VolumeSystem& coordSystem_;
};
}
}  // namespace om::tile::
