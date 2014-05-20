#pragma once

#include "datalayer/file.h"
#include "volume/ivolume.hpp"
#include "volume/iterators.hpp"
#include "tile/dataSources.hpp"
#include "chunk/dataSources.hpp"

namespace om {

namespace tile {
class CachedDataSource;
}

namespace chunk {
class CachedDataSource;
}

namespace volume {
class MetadataDataSource;
class MetadataManager;
class MetadataDataSource;
struct Metadata;

// Used too much as is in server etc?

class Volume : virtual public IVolume {
 public:
  Volume(file::path uri);
  virtual ~Volume();

  tile::TileDS& TileDS() const override;
  chunk::ChunkDS& ChunkDS() const override;

  MetadataManager& Metadata() const override;
  const coords::VolumeSystem& Coords() const override;
  coords::DataBbox Bounds() const;

  void CloseDownThreads();

  // Probably refactor out?
  int GetBytesPerVoxel() const;

  const std::string& GUID() const override;
  const std::string& Endpoint() const override;
  const std::string& Name() const override;

  template <typename T>
  struct iterable_volume {
    all_dataval_iterator<T> begin() {
      return make_all_dataval_iterator<T>(bounds, vol.ChunkDS());
    }
    all_dataval_iterator<T> end() { return all_dataval_iterator<T>(); }
    const Volume& vol;
    coords::DataBbox bounds;
  };

  template <typename T>
  iterable_volume<T> Iterate() const {
    return Volume::iterable_volume<T>{*this, Bounds()};
  }
  template <typename T>
  iterable_volume<T> Iterate(coords::DataBbox bounds) const {
    return Volume::iterable_volume<T>{*this, bounds};
  }
  template <typename T>
  iterable_volume<T> Iterate(coords::GlobalBbox bounds) const {
    return Volume::iterable_volume<T>{*this, bounds.ToDataBbox(Coords(), 0)};
  }

 protected:
  std::unique_ptr<MetadataDataSource> metaDS_;
  std::unique_ptr<MetadataManager> metaManager_;

  std::unique_ptr<tile::CachedDataSource> tileDS_;
  std::unique_ptr<chunk::CachedDataSource> chunkDS_;
};
}
}  // namespace om::volume::
