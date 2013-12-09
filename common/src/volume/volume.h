#pragma once

#include <boost/filesystem.hpp>

#include "volume/ivolume.hpp"
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
  Volume(boost::filesystem::path uri);
  virtual ~Volume();

  tile::TileDS& TileDS() const override;
  chunk::ChunkDS& ChunkDS() const override;

  MetadataManager& Metadata() const override;
  const coords::VolumeSystem& Coords() const override;

  void CloseDownThreads();

  // Probably refactor out?
  int GetBytesPerVoxel() const;

  const std::string& GUID() const override;
  const std::string& Endpoint() const override;
  const std::string& Name() const override;

 protected:
  std::unique_ptr<MetadataDataSource> metaDS_;
  std::unique_ptr<MetadataManager> metaManager_;

  std::unique_ptr<tile::CachedDataSource> tileDS_;
  std::unique_ptr<chunk::CachedDataSource> chunkDS_;
};
}
}  // namespace om::volume::
